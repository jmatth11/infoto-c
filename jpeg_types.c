#include "jpeg_types.h"

#include <jpeglib.h>
#include <setjmp.h>

/**
* interface
*
  int (*get_num_components)(void *);
  int (*get_image_width)(void *);
  int (*get_image_height)(void *);
  int (*get_color_space)(void *);
  double (*get_gamma)(void *);
  void (*set_num_components)(void *, int);
  void (*set_image_width)(void *, int);
  void (*set_image_height)(void *, int);
  void (*set_color_space)(void *, int);
  void (*set_gamma)(void *, double);

  */

/**
 * Custom jpeg err structure
 */
struct jpeg_err {
  struct jpeg_error_mgr pub;

  jmp_buf jmp_to_err_handler;
};

/**
 * Structure to hold decompression jpeg image info.
 */
struct decomp_img {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_err err;
};

/**
 * Structure to hold compression jpeg image info.
 */
struct comp_img {
  struct jpeg_compress_struct cinfo;
  struct jpeg_err err;
};

int get_decomp_num_components(void *decomp) {
  struct decomp_img *local = (struct decomp_img *)decomp;
  return local->cinfo.num_components;
}
int get_decomp_image_width(void *decomp) {
  struct decomp_img *local = (struct decomp_img *)decomp;
  return local->cinfo.image_width;
}

int get_decomp_image_height(void *decomp) {
  struct decomp_img *local = (struct decomp_img *)decomp;
  return local->cinfo.image_height;
}

int get_decomp_color_space(void *decomp) {
  struct decomp_img *local = (struct decomp_img *)decomp;
  return local->cinfo.out_color_space;
}

double get_decomp_gamma(void *decomp) {
  struct decomp_img *local = (struct decomp_img *)decomp;
  return local->cinfo.output_gamma;
}

void set_decomp_num_components(void *decomp, int nc) {
  struct decomp_img *local = (struct decomp_img *)decomp;
  local->cinfo.output_components = nc;
}

void set_decomp_image_width(void *decomp, int img_w) {
  struct decomp_img *local = (struct decomp_img *)decomp;
  local->cinfo.output_width = img_w;
}

void set_decomp_image_height(void *decomp, int img_h) {
  struct decomp_img *local = (struct decomp_img *)decomp;
  local->cinfo.output_height = img_h;
}

void set_decomp_color_space(void *decomp, int cs) {
  struct decomp_img *local = (struct decomp_img *)decomp;
  local->cinfo.out_color_space = cs;
}

void set_decomp_gamma(void *decomp, double g) {
  struct decomp_img *local = (struct decomp_img *)decomp;
  local->cinfo.output_gamma = g;
}

int get_comp_num_components(void *comp) {
  struct comp_img *local = (struct comp_img *)comp;
  return local->cinfo.input_components;
}
int get_comp_image_width(void *comp) {
  struct comp_img *local = (struct comp_img *)comp;
  return local->cinfo.image_width;
}

int get_comp_image_height(void *comp) {
  struct comp_img *local = (struct comp_img *)comp;
  return local->cinfo.image_height;
}

int get_comp_color_space(void *comp) {
  struct comp_img *local = (struct comp_img *)comp;
  return local->cinfo.in_color_space;
}

double get_comp_gamma(void *comp) {
  struct comp_img *local = (struct comp_img *)comp;
  return local->cinfo.input_gamma;
}

void set_comp_num_components(void *comp, int nc) {
  struct comp_img *local = (struct comp_img *)comp;
  local->cinfo.input_components = nc;
}

void set_comp_image_width(void *comp, int img_w) {
  struct comp_img *local = (struct comp_img *)comp;
  local->cinfo.image_width = img_w;
}

void set_comp_image_height(void *comp, int img_h) {
  struct comp_img *local = (struct comp_img *)comp;
  local->cinfo.image_height = img_h;
}

void set_comp_color_space(void *comp, int cs) {
  struct comp_img *local = (struct comp_img *)comp;
  local->cinfo.in_color_space = cs;
}

void set_comp_gamma(void *comp, double g) {
  struct comp_img *local = (struct comp_img *)comp;
  local->cinfo.input_gamma = g;
}

/**
 * Initialize a JPEG decompression object.
 *
 * @param[in] filename The filename to use.
 * @param[out] decomp The image file object.
 * @returns True on success, false otherwise.
 */
bool init_jpeg_decomp(const char *filename, infoto_img_file *decomp) {
  // open file
  if ((decomp->file = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return false;
  }
  // TODO finish initializing these objects
  // set up error handler
  // decomp->cinfo.err = jpeg_std_error(&decomp->err.pub);
  // decomp->err.pub.error_exit = handle_read_error;
  //// create decompress object
  // jpeg_create_decompress(&decomp->cinfo);
  //// set up the std in source (our file)
  // jpeg_stdio_src(&decomp->cinfo, decomp->file);
  //// read in and populate the header files
  // jpeg_read_header(&decomp->cinfo, true);
  // return true;
}

/**
 * Initialize a JPEG compression object.
 *
 * @param[in] filename The filename to use.
 * @param[out] comp The image file object.
 * @returns True on success, false otherwise.
 */
bool init_jpeg_comp(const char *filename, infoto_img_file *comp) {

  return true;
}
