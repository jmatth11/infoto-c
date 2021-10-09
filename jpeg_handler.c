#include "jpeg_handler.h"
#include "img_utils.h"
#include "str_utils.h"

#include <jpeglib.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

// temp file name constant values
#define TMP_FILE_NAME "-edited.tmp"
#define TMP_FILE_NAME_LEN strlen(TMP_FILE_NAME)

#define EDITED_FILE_NAME "-edited"
#define EDITED_FILE_NAME_LEN strlen(EDITED_FILE_NAME)

/**
 * Custom jpeg err structure
 */
struct jpeg_err {
  struct jpeg_error_mgr pub;

  jmp_buf jmp_to_err_handler;
};

/**
 * custom error handler for jpeg error.
 *
 * @param[in] cinfo The common jpeg object
 */
static void handle_read_error(j_common_ptr cinfo) {
  struct jpeg_err *err = (struct jpeg_err *)cinfo->err;

  (*cinfo->err->output_message)(cinfo);

  longjmp(err->jmp_to_err_handler, 1);
}

/**
 * Structure to hold decompression jpeg image info.
 */
struct decomp_img {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_err err;
  FILE *file;
};

/**
 * Structure to hold compression jpeg image info.
 */
struct comp_img {
  struct jpeg_compress_struct cinfo;
  struct jpeg_err err;
  FILE *file;
};

bool decomp_img(const char *file_name, struct decomp_img *decomp) {
  // open file
  if ((decomp->file = fopen(file_name, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", file_name);
    return false;
  }
  // set up error handler
  decomp->cinfo.err = jpeg_std_error(&decomp->err.pub);
  decomp->err.pub.error_exit = handle_read_error;
  // create decompress object
  jpeg_create_decompress(&decomp->cinfo);
  // set up the std in source (our file)
  jpeg_stdio_src(&decomp->cinfo, decomp->file);
  // read in and populate the header files
  jpeg_read_header(&decomp->cinfo, true);
  return true;
}

void close_jpeg_img(j_common_ptr info) {
  // finish up objects
  if (info->is_decompressor) {
    jpeg_finish_decompress((j_decompress_ptr)info);
  } else {
    jpeg_finish_compress((j_compress_ptr)info);
  }
  // destroy them
  jpeg_destroy(info);
}

void clean_up(struct comp_img *comp, struct decomp_img *decomp) {
  // close jpen imgs
  close_jpeg_img((j_common_ptr)&comp->cinfo);
  close_jpeg_img((j_common_ptr)&decomp->cinfo);
  // close the files if they are open
  if (comp->file != NULL) {
    fclose(comp->file);
  }
  if (decomp->file != NULL) {
    fclose(decomp->file);
  }
}

bool comp_img(const char *file_name, struct comp_img *comp) {
  printf("comp_img\n");
  // set up the error handler
  comp->cinfo.err = jpeg_std_error(&comp->err.pub);
  // create the compress object
  jpeg_create_compress(&comp->cinfo);
  // open the file to write to
  if ((comp->file = fopen(file_name, "wb")) == NULL) {
    fprintf(stderr, "can't open file: %s\n", file_name);
    return false;
  }
  // set our std out destination (the file)
  jpeg_stdio_dest(&comp->cinfo, comp->file);
  printf("end comp_img\n");
  return true;
}

void sync_settings(const int added_pixels, struct decomp_img *decomp,
                   struct comp_img *comp) {
  // grab our decomp img's width and height + the specified added pixels
  const int border = added_pixels * 2;
  comp->cinfo.image_width = decomp->cinfo.image_width + border;
  printf("cinfo.image_width = %d\n", comp->cinfo.image_width);
  comp->cinfo.image_height = decomp->cinfo.image_height + border;
  printf("cinfo.image_height = %d\n", comp->cinfo.image_height);
  // grab the number of color components
  comp->cinfo.input_components = decomp->cinfo.num_components;
  printf("cinfo.input_components = %d\n", comp->cinfo.input_components);
  // grab the color space value. must be out_color_space
  comp->cinfo.in_color_space = decomp->cinfo.out_color_space;
  printf("cinfo.in_color_space = %d\n", comp->cinfo.in_color_space);
  comp->cinfo.input_gamma = decomp->cinfo.output_gamma;
  printf("cinfo.input_gamma = %f\n", comp->cinfo.input_gamma);
  // set the rest of the defaults
  jpeg_set_defaults(&comp->cinfo);
  // keep original quality
  jpeg_set_quality(&comp->cinfo, 100, true);
}

char *tmp_file_name(const char *file_name) {
  int len = strlen(file_name);
  char *tmp_file_name =
      (char *)malloc((len + TMP_FILE_NAME_LEN + 1) * sizeof(char));
  strncpy(tmp_file_name, file_name, len);
  memcpy(&tmp_file_name[len], TMP_FILE_NAME, TMP_FILE_NAME_LEN);
  // memcpy doesn't add null terminator. so add it.
  tmp_file_name[len + TMP_FILE_NAME_LEN] = '\0';
  return tmp_file_name;
}

/**
 * Copy image data from decomp into comp and handle border creation.
 *
 * @param[in] cfg The configuration object.
 * @param[in] border_color The border color to use.
 * @param[in] decomp The decompressed image to read from.
 * @param[in,out] comp The compressed image to write to.
 */
void copy_read_data_to_write_buffer(const config *cfg, const pixel border_color,
                                    struct decomp_img *decomp,
                                    struct comp_img *comp) {
  int num_comp = comp->cinfo.input_components;
  int row_size = comp->cinfo.image_width * num_comp;
  printf("row_size = %d\n", row_size);

  JSAMPROW row_stride = (JSAMPLE *)malloc(row_size * sizeof(JSAMPLE));
  JSAMPARRAY row_array = &row_stride;

  int read_width = decomp->cinfo.image_width * decomp->cinfo.num_components;
  printf("read_width = %d\n", read_width);
  int border_side_width = cfg->background.pixels * num_comp;
  int border_and_read_width = read_width + border_side_width;

  // this buffer gets cleaned up when decomp's cinfo gets cleaned up.
  JSAMPARRAY buffer = (*(decomp->cinfo).mem->alloc_sarray)(
      (j_common_ptr)&decomp->cinfo, JPOOL_IMAGE, read_width, 1);

  while (decomp->cinfo.output_scanline < decomp->cinfo.output_height) {
    for (int i = 0; i < border_side_width; i += num_comp) {
      write_pixel_to_buffer(border_color, i, row_stride);
    }
    // TODO colors are bright pink. bytes might be off
    jpeg_read_scanlines(&decomp->cinfo, buffer, 1);
    memcpy(&row_stride[border_side_width], buffer[0], read_width);

    for (int i = border_and_read_width;
         i < (border_and_read_width + border_side_width); i += num_comp) {
      write_pixel_to_buffer(border_color, i, row_stride);
    }
    jpeg_write_scanlines(&comp->cinfo, row_array, 1);
  }
  free(row_stride);
}

bool init_jpeg_objects(const config *cfg, const char *tmp_file,
                       struct decomp_img *decomp, struct comp_img *comp) {
  // initialize decomp
  if (!decomp_img(cfg->img, decomp)) {
    printf("failed to read jpeg image\n");
    return false;
  }
  // initialize comp
  if (!comp_img(tmp_file, comp)) {
    printf("failed creating jpeg writer\n");
    return false;
  }
  // sync settings
  sync_settings(cfg->background.pixels, decomp, comp);
  return true;
}

bool write_jpeg_row(const void *input, uint8_t *buf, void *image) {
  const config *cfg = (const config *)input;
  struct comp_img *comp = (struct comp_img *)image;
  JSAMPARRAY row_array = &buf;
  for (int i = 0; i < cfg->background.pixels; ++i) {
    jpeg_write_scanlines(&comp->cinfo, row_array, 1);
  }
  return true;
}

void init_stdio_jpeg_writer(const config *cfg, struct comp_img *comp,
                            infoto_img_writer *writer) {
  writer->image_width = comp->cinfo.image_width;
  writer->num_components = comp->cinfo.num_components;
  writer->write_row = &write_jpeg_row;
}

/**
 * Add text and border to image, given config and info text.
 *
 * @param[in] cfg The config object
 * @param[in] info The info text object
 * @return True if successful, False otherwise
 */
bool add_text_to_img(const config *cfg, const info_text *info) {
  // create reader for img
  struct decomp_img decomp;
  memset(&decomp, 0, sizeof(decomp));
  // create writer for img
  struct comp_img comp;
  memset(&comp, 0, sizeof(comp));
  // create tmp edit file name
  // TODO change this to be the only name, we do not need a tmp file name and
  // edited name
  char *tmp_edit_file_name = tmp_file_name(cfg->img);
  // set up error handling for decomp and comp structs
  if (setjmp(decomp.err.jmp_to_err_handler) ||
      setjmp(comp.err.jmp_to_err_handler)) {
    clean_up(&comp, &decomp);
    free(tmp_edit_file_name);
    return false;
  }

  if (!init_jpeg_objects(cfg, tmp_edit_file_name, &decomp, &comp)) {
    return false;
  }
  bool result = true;
  pixel border_color = get_colored_pixel(cfg->background.color);
  border_color.use_alpha = 0;
  if (comp.cinfo.input_components > 3)
    border_color.use_alpha = 1;

  infoto_img_writer background_writer;
  init_stdio_jpeg_writer(cfg, &comp, &background_writer);

  // write border and original image
  jpeg_start_compress(&comp.cinfo, true);

  // TODO figure out if this needs to be changed anymore
  write_background_rows(&background_writer, cfg, &comp, border_color);

  jpeg_start_decompress(&decomp.cinfo);

  // TODO refactor
  copy_read_data_to_write_buffer(cfg, border_color, &decomp, &comp);

  write_background_rows(&background_writer, cfg, &comp, border_color);

  // save new image
  // clean up writer, reader
  clean_up(&comp, &decomp);

  // TODO refactor this section because it sucks
  // get pointer to extension position
  const char *start_of_extension = get_filename_ext(cfg->img);
  // calculate lengths of things
  int img_file_name_size = strlen(cfg->img);
  int edited_file_name_size = img_file_name_size + EDITED_FILE_NAME_LEN + 1;
  int file_name_no_ext_size = (img_file_name_size - strlen(start_of_extension));
  char *edited_file_name = (char *)malloc(edited_file_name_size * sizeof(char));
  strncpy(edited_file_name, cfg->img, file_name_no_ext_size);
  memcpy(&edited_file_name[file_name_no_ext_size], EDITED_FILE_NAME,
         EDITED_FILE_NAME_LEN);
  memcpy(&edited_file_name[file_name_no_ext_size + EDITED_FILE_NAME_LEN],
         start_of_extension, strlen(start_of_extension));
  // add null terminator
  edited_file_name[edited_file_name_size] = '\0';

  // rename file
  if (rename(tmp_edit_file_name, edited_file_name) != 0) {
    printf("rename file failed.\n");
    result = false;
  }
  free(tmp_edit_file_name);
  free(edited_file_name);
  return result;
}
