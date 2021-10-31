#include <stdbool.h>
#include <stdio.h>

#include "config.h"
#include "info_text.h"
#include "jpeg_handler.h"
#include "str_utils.h"

#include <jpeglib.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

// temp file name constant values
#define TMP_FILE_NAME "-edited.tmp"
#define TMP_FILE_NAME_LEN strlen(TMP_FILE_NAME)

/**
 * JPEG handler structure.
 */
struct infoto_jpeg_handler {
  infoto_font_handler *font_handler;
};

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

/**
 * Initialize decomp_img.
 *
 * @param[in] file_name The filename the decompressed image should open.
 * @param[out] decomp The decompressed image to initialize.
 * @returns True on success, false otherwise.
 */
static bool init_decomp_img(const char *file_name, struct decomp_img *decomp) {
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

/**
 * Initialize comp_img.
 *
 * @param[in] file_name The filename the compressed image should open.
 * @param[out] comp The compressed image to initialize.
 * @returns True on success, false otherwise.
 */
static bool init_comp_img(const char *file_name, struct comp_img *comp) {
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
  return true;
}

static void close_jpeg_img(j_common_ptr info) {
  // finish up objects
  if (info->is_decompressor) {
    jpeg_finish_decompress((j_decompress_ptr)info);
  } else {
    jpeg_finish_compress((j_compress_ptr)info);
  }
  // destroy them
  jpeg_destroy(info);
}

/**
 * Convenience function to clean up comp_img, decomp_img, and edited file name.
 *
 * @param[out] comp The compressed image.
 * @param[out] decomp The decompressed image.
 * @param[out] edit_name The filename for the edited image.
 */
static void clean_up(struct comp_img *comp, struct decomp_img *decomp,
                     char *edit_name) {
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
  if (edit_name != NULL) {
    free(edit_name);
  }
}

/**
 * Sync JPEG settings between compression and decompression images.
 * The compressed image will inherit the settings of the decompressed image.
 *
 * @param[in] added_pixels Number of pixels for the border around the image.
 * @param[in] decomp The decompressed image.
 * @param[in,out] comp The compressed image.
 */
static void sync_settings(const int added_pixels,
                          const struct decomp_img *decomp,
                          struct comp_img *comp) {
  // grab our decomp img's width and height + the specified added pixels
  const int border = added_pixels * 2;
  comp->cinfo.image_width = decomp->cinfo.image_width + border;
  comp->cinfo.image_height = decomp->cinfo.image_height + border;
  // grab the number of color components
  comp->cinfo.input_components = decomp->cinfo.num_components;
  // grab the color space value. must be out_color_space
  comp->cinfo.in_color_space = decomp->cinfo.out_color_space;
  comp->cinfo.input_gamma = decomp->cinfo.output_gamma;
  // set the rest of the defaults
  jpeg_set_defaults(&comp->cinfo);
  // keep original quality
  jpeg_set_quality(&comp->cinfo, 100, true);
}

// TODO rework this to be generic using infoto_img_file objects
/**
 * Copy image data from decomp into comp and handle border creation.
 *
 * @param[in] background The background info.
 * @param[in] border_color The border color to use.
 * @param[in] decomp The decompressed image to read from.
 * @param[in,out] comp The compressed image to write to.
 */
static void copy_read_data_to_write_buffer(const background_info background,
                                           const pixel border_color,
                                           struct decomp_img *decomp,
                                           struct comp_img *comp) {
  int num_comp = comp->cinfo.input_components;
  int row_size = comp->cinfo.image_width * num_comp;

  JSAMPROW row_stride = (JSAMPLE *)malloc(row_size * sizeof(JSAMPLE));
  JSAMPARRAY row_array = &row_stride;

  int read_width = decomp->cinfo.image_width * decomp->cinfo.num_components;
  int border_side_width = background.pixels * num_comp;
  int border_and_read_width = read_width + border_side_width;

  // this buffer gets cleaned up when decomp's cinfo gets cleaned up.
  JSAMPARRAY buffer = (*(decomp->cinfo).mem->alloc_sarray)(
      (j_common_ptr)&decomp->cinfo, JPOOL_IMAGE, read_width, 1);

  while (decomp->cinfo.output_scanline < decomp->cinfo.output_height) {
    // writing side border
    for (int i = 0; i < border_side_width; i += num_comp) {
      infoto_write_pixel_to_buffer(border_color, i, row_stride);
    }
    // read in data from decompressed jpeg file
    jpeg_read_scanlines(&decomp->cinfo, buffer, 1);
    memcpy(&row_stride[border_side_width], buffer[0], read_width);
    // writing other side border
    for (int i = border_and_read_width;
         i < (border_and_read_width + border_side_width); i += num_comp) {
      infoto_write_pixel_to_buffer(border_color, i, row_stride);
    }
    // write out to comressed jpeg file
    jpeg_write_scanlines(&comp->cinfo, row_array, 1);
  }
  free(row_stride);
}

/**
 * Initialize JPEG objects.
 *
 * @param[in] filename The original filename.
 * @param[in] pixel_count The pixel count for the border.
 * @param[in] out_file Filename of file to write out to.
 * @param[out] decomp The decomp_img object to initialize.
 * @param[out] comp The comp_img object to initialize.
 * @returns True on success, false otherwise.
 */
static bool init_jpeg_objects(const char *filename, const int pixel_count,
                              const char *out_file, struct decomp_img *decomp,
                              struct comp_img *comp) {
  // initialize decomp
  if (!init_decomp_img(filename, decomp)) {
    fprintf(stderr, "failed to read jpeg image\n");
    return false;
  }
  // initialize comp
  if (!init_comp_img(out_file, comp)) {
    fprintf(stderr, "failed creating jpeg writer\n");
    return false;
  }
  // sync settings
  sync_settings(pixel_count, decomp, comp);
  // start compress and decompress objects
  jpeg_start_compress(&comp->cinfo, true);
  jpeg_start_decompress(&decomp->cinfo);
  return true;
}

/**
 * Write a given buffer to a JPEG image.
 *
 * @param[in] background The background info.
 * @param[in] buf The buffer to write out.
 * @param[in,out] image The compression image object to write to.
 * @returns True for success, false otherwise.
 */
static bool write_jpeg_matrix(const background_info background, uint8_t **buf,
                              void *image) {
  struct comp_img *comp = (struct comp_img *)image;
  jpeg_write_scanlines(&comp->cinfo, buf, background.pixels);
  return true;
}

/**
 * Initialize JPEG writer that uses stdio for writing.
 *
 * @param[in] comp The comp_img structure.
 * @param[out] writer The infoto_img_writer to initialize.
 */
static void init_jpeg_writer(struct comp_img *comp, infoto_img_writer *writer) {
  writer->image_width = comp->cinfo.image_width;
  writer->num_components = comp->cinfo.num_components;
  writer->write_matrix = &write_jpeg_matrix;
}

/**
 * Handle generating the new JPEG file from the given info.
 *
 * @param[in] background_writer The writer for the background color.
 * @param[in,out] comp The compressed JPEG image.
 * @param[in] decomp The decompressed JPEG image.
 * @param[in] background The background info.
 * @param[in] border_color The color to use for the border.
 * @param[in] glyph_str The glyph string to write out.
 * @returns True if successful, false otherwise.
 */
static bool handle_jpeg_copying(infoto_img_writer *background_writer,
                                struct comp_img *comp,
                                struct decomp_img *decomp,
                                const background_info background,
                                const pixel border_color,
                                const infoto_glyph_str *glyph_str) {
  // TODO figure out if this needs to be changed anymore
  // don't write out glyph string on top border
  if (!infoto_write_background_rows(background_writer, comp, background,
                                    border_color, NULL)) {
    return false;
  }
  // TODO refactor
  copy_read_data_to_write_buffer(background, border_color, decomp, comp);
  // write out glyph string on bottom border
  if (!infoto_write_background_rows(background_writer, comp, background,
                                    border_color, glyph_str)) {
    return false;
  }
  return true;
}

/**
 * Write out border and text info to a given JPEG image.
 * This function does not overwrite the original image but makes a new edited
 * image file.
 *
 * @param[in] filename The original filename.
 * @param[in] background The background info.
 * @param[in] info The info text object
 * @return True if successful, False otherwise
 */
static bool write_jpeg_image(infoto_img_handler *handler, const char *filename,
                             const background_info background,
                             const info_text *info) {

  struct infoto_jpeg_handler *jpeg_handler =
      (struct infoto_jpeg_handler *)handler->_internal;
  // create reader for img
  struct decomp_img decomp;
  memset(&decomp, 0, sizeof(decomp));
  // create writer for img
  struct comp_img comp;
  memset(&comp, 0, sizeof(comp));
  // create edit file name
  char *edit_file_name = infoto_get_edit_file_name(filename);
  // set up error handling for decomp and comp structs
  if (setjmp(decomp.err.jmp_to_err_handler) ||
      setjmp(comp.err.jmp_to_err_handler)) {
    clean_up(&comp, &decomp, edit_file_name);
    return false;
  }
  bool success = true;
  if (!init_jpeg_objects(filename, background.pixels, edit_file_name, &decomp,
                         &comp)) {
    clean_up(&comp, &decomp, edit_file_name);
    return false;
  }
  pixel border_color = infoto_get_colored_pixel(background.color);
  border_color.use_alpha = comp.cinfo.input_components == 4 ? true : false;

  // generate glyph string from info text
  infoto_glyph_str *glyph_str;
  infoto_glyph_str_init(&glyph_str);
  char *info_str = infoto_info_text_to_string(info);
  success = infoto_create_glyph_str_from_text(jpeg_handler->font_handler,
                                              glyph_str, info_str);
  // free the info_str
  free(info_str);
  if (!success) {
    // TODO this doesn't handle freeing other initialized objects
    fprintf(stderr, "failed to create glyph string from text.\n");
    return false;
  }

  infoto_img_writer background_writer;
  init_jpeg_writer(&comp, &background_writer);

  success = handle_jpeg_copying(&background_writer, &comp, &decomp, background,
                                border_color, glyph_str);
  // free the glyph string
  infoto_glyph_str_free(glyph_str);
  glyph_str = NULL;
  // save new image
  // clean up writer, reader, and edit file name
  clean_up(&comp, &decomp, edit_file_name);
  return success;
}

/**
 * Initialize a infoto JPEG handler in the given img handler interface.
 *
 * @param[out] img_handler The image handler interface to populate.
 * @param[in] font_handler The font handler for the JPEG handler to reference.
 */
void infoto_jpeg_handler_init(infoto_img_handler *img_handler,
                              infoto_font_handler *font_handler) {
  struct infoto_jpeg_handler *local =
      (struct infoto_jpeg_handler *)malloc(sizeof(struct infoto_jpeg_handler));
  local->font_handler = font_handler;
  img_handler->_internal = local;
  img_handler->write_image = write_jpeg_image;
}

/**
 * Free the internal JPEG handler.
 * This function does not free the font handler given at initialization.
 *
 * @param[out] img_handler The img handler to free.
 */
void infoto_jpeg_handler_free(infoto_img_handler *img_handler) {
  struct infoto_jpeg_handler *local =
      (struct infoto_jpeg_handler *)img_handler->_internal;
  local->font_handler = NULL;
  free(local);
}
