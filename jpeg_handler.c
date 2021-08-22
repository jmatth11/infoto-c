#include "jpeg_handler.h"

#include <jpeglib.h>
#include <setjmp.h>
#include <string.h>

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

// JSAMPARRAY buffer;
// int row_stride;
// jpeg_start_decompress(cinfo);

// row_stride = cinfo->output_width * cinfo->output_components;
// buffer = (*cinfo->mem->alloc_sarray)((j_common_ptr)cinfo, JPOOL_IMAGE,
//                                     row_stride, 1);

// while (cinfo->output_scanline < cinfo->output_height) {
//  jpeg_read_scanlines(cinfo, buffer, 1);
//}

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

void clean_up(struct comp_img *comp, struct decomp_img *decomp, char *name) {
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
  // free the malloced tmp name
  if (name != NULL) {
    free(name);
  }
}

bool comp_img(const char *file_name, struct comp_img *comp) {
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

void sync_settings(const int added_pixels, struct decomp_img *decomp,
                   struct comp_img *comp) {
  // grab our decomp img's width and height + the specified added pixels
  comp->cinfo.image_width = decomp->cinfo.image_width + added_pixels;
  comp->cinfo.image_height = decomp->cinfo.image_height + added_pixels;
  // grab the number of color components
  comp->cinfo.input_components = decomp->cinfo.num_components;
  // grab the color space value
  comp->cinfo.in_color_space = decomp->cinfo.jpeg_color_space;
  // set the rest as defaults
  jpeg_set_defaults(&comp->cinfo);
  // keep original quality
  jpeg_set_quality(&comp->cinfo, 100, true);
}

char *tmp_file_name(const char *file_name) { return NULL; }

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
  char *edit_file_name = tmp_file_name(cfg->img);
  // set up error handling for decomp and comp structs
  if (setjmp(decomp.err.jmp_to_err_handler) ||
      setjmp(comp.err.jmp_to_err_handler)) {
    clean_up(&comp, &decomp, edit_file_name);
    return false;
  }
  // initialize decomp
  if (!decomp_img(cfg->img, &decomp)) {
    printf("failed to read jpeg image\n");
    return false;
  }
  // initialize comp
  if (!comp_img(edit_file_name, &comp)) {
    printf("failed creating jpeg writer\n");
    return false;
  }
  // sync settings
  sync_settings(cfg->background.pixels, &decomp, &comp);
  // write border and original image

  // save new image
  // clean up writer, reader, and new file name
  clean_up(&comp, &decomp, edit_file_name);
  return true;
}
