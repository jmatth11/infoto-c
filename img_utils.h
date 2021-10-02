#ifndef INFOTO_IMG_UTILS
#define INFOTO_IMG_UTILS

#include <stdint.h>

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

typedef enum {
  BACKGROUND_BLACK,
  BACKGROUND_BLUE,
  BACKGROUND_GREEN,
  BACKGROUND_RED,
  BACKGROUND_WHITE
} background_color;

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

#endif
