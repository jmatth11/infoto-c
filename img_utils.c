#include "img_utils.h"

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
