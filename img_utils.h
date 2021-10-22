#ifndef INFOTO_IMG_UTILS
#define INFOTO_IMG_UTILS

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "config.h"
#include "info_text.h"
#include "ttf_util.h"

/**
 * Infoto Image Handler interface.
 * Handles everything necessary to write info text to an image.
 */
struct infoto_img_handler {
  void *_internal;
  bool (*write_image)(struct infoto_img_handler *, const char *,
                      const background_info, const info_text *);
};
typedef struct infoto_img_handler infoto_img_handler;

/**
 * Structure to represent a pixel in an image.
 */
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t alpha;
  uint8_t use_alpha;
} pixel;

/**
 * Function pointer type for writing a matrix to an image.
 */
typedef bool (*write_matrix_fn)(const background_info, uint8_t **, void *);

/**
 * Generic writer object for an image.
 */
typedef struct {
  int image_width;
  int num_components;
  write_matrix_fn write_matrix;
} infoto_img_writer;

/**
 * Get background_color enum from the given string.
 *
 * @param[in] s Name of the color.
 * @return background_color from the given string, BACKGROUND_WHITE if the
 * name cannot be resolved.
 */
background_color get_background_color_from_string(const char *s);

/**
 * Get a pixel structure that represents the given background color.
 *
 * @param[in] bgc The background color enum.
 * @return pixel structure that represents the background color enum.
 */
pixel get_colored_pixel(const background_color bgc);

/**
 * Write pixel info out to the given buffer.
 *
 * @param[in] p pixel structure to write out.
 * @param[in] i Index to start writing from in buffer.
 * @param[in,out] buf Buffer to write to.
 * @return Number of bytes written out.
 */
int write_pixel_to_buffer(const pixel p, const int i, uint8_t *buf);

/**
 * Write out background border of given color to image writer.
 *
 * @param[in] writer The writer.
 * @param[in,out] image The object that manages image data.
 * @param[in] background The background information.
 * @param[in] color The pixel color for the background border.
 * @param[in] glyph_str The glyph string to write out to the background. Pass
 * NULL if nothing should be written out.
 * @returns True if everything was successful, false otherwise.
 */
bool write_background_rows(infoto_img_writer *writer, void *image,
                           const background_info background, const pixel color,
                           infoto_glyph_str *glyph_str);

#endif
