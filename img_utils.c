#include "img_utils.h"

#include <stdlib.h>
#include <string.h>

#define INFOTO_BACKGROUND_BLACK "black"
#define INFOTO_BACKGROUND_BLUE "blue"
#define INFOTO_BACKGROUND_GREEN "green"
#define INFOTO_BACKGROUND_RED "red"
#define INFOTO_BACKGROUND_WHITE "white"

/**
 * Get background_color enum from the given string.
 *
 * @param[in] s Name of the color.
 * @return background_color from the given string, BACKGROUND_WHITE is default
 * if the name cannot be resolved.
 */
background_color get_background_color_from_string(const char *s) {
  if (strcmp(INFOTO_BACKGROUND_BLACK, s) == 0)
    return BACKGROUND_BLACK;
  if (strcmp(INFOTO_BACKGROUND_BLUE, s) == 0)
    return BACKGROUND_BLUE;
  if (strcmp(INFOTO_BACKGROUND_GREEN, s) == 0)
    return BACKGROUND_GREEN;
  if (strcmp(INFOTO_BACKGROUND_RED, s) == 0)
    return BACKGROUND_RED;
  // whether it's white or not, return white
  return BACKGROUND_WHITE;
}

/**
 * Get a pixel structure that represents the given background color.
 *
 * @param[in] bgc The background color enum.
 * @return pixel structure that represents the background color enum.
 */
pixel get_colored_pixel(const background_color bgc) {
  pixel p;
  p.alpha = 255;
  switch (bgc) {
  case BACKGROUND_BLACK:
    p.r = 0;
    p.g = 0;
    p.b = 0;
    break;
  case BACKGROUND_BLUE:
    p.r = 0;
    p.g = 0;
    p.b = 255;
    break;
  case BACKGROUND_GREEN:
    p.r = 0;
    p.g = 255;
    p.b = 0;
    break;
  case BACKGROUND_RED:
    p.r = 255;
    p.g = 0;
    p.b = 0;
    break;
  case BACKGROUND_WHITE:
    p.r = 255;
    p.g = 255;
    p.b = 255;
    break;
  }
  return p;
}

/**
 * Write pixel info out to the given buffer.
 *
 * @param[in] p pixel structure to write out.
 * @param[in] i Index to start writing from in buffer.
 * @param[in,out] buf Buffer to write to.
 * @return Number of bytes written out.
 */
int write_pixel_to_buffer(const pixel p, const int i, uint8_t *buf) {
  buf[i] = p.r;
  buf[i + 1] = p.g;
  buf[i + 2] = p.b;
  if (p.use_alpha) {
    buf[i + 3] = p.alpha;
    return 4;
  }
  return 3;
}

bool write_glyph_str(uint8_t **matrix_buf, int num_components, int row_size,
                     int height, infoto_glyph_str *glyph_str) {
  // int glyph_left_pos = 0;
  // int glyph_top_pos = 0;
  // if (glyph_str != NULL) {
  //   int glyph_width = infoto_glyph_str_get_width(glyph_str) * num_components;
  //   int glyph_height = infoto_glyph_str_get_height(glyph_str);
  //   glyph_left_pos = (row_size / 2) - (glyph_width / 2);
  //   glyph_top_pos = (height / 2) - (glyph_height / 2);
  // }
  // TODO actually write out the glyph string
  return true;
}

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
bool write_background_rows(infoto_img_writer *writer, void *data,
                           const background_info background, const pixel color,
                           infoto_glyph_str *glyph_str) {
  bool result = true;
  // create matrix of img data
  uint8_t **matrix_buf =
      (uint8_t **)malloc(background.pixels * sizeof(uint8_t *));
  int row_size = writer->image_width * writer->num_components;
  for (int i = 0; i < background.pixels; ++i) {
    matrix_buf[i] = (uint8_t *)malloc(row_size * sizeof(uint8_t));
    for (int j = 0; j < row_size; j += writer->num_components) {
      write_pixel_to_buffer(color, j, matrix_buf[i]);
    }
  }
  if (glyph_str != NULL) {
    result = write_glyph_str(matrix_buf, writer->num_components, row_size,
                             background.pixels, glyph_str);
  }
  if (result && !writer->write_matrix(background, matrix_buf, data)) {
    result = false;
  }
  // free arrays
  for (int i = 0; i < background.pixels; ++i) {
    free(matrix_buf[i]);
  }
  free(matrix_buf);
  return result;
}
