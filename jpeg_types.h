#ifndef INFOTO_JPEG_TYPES
#define INFOTO_JPEG_TYPES

#include <stdbool.h>

#include "img_utils.h"

/**
 * Initialize a JPEG decompression object.
 *
 * @param[in] filename The filename to use.
 * @param[out] decomp The image file object.
 * @returns True on success, false otherwise.
 */
bool init_jpeg_decomp(const char *filename, infoto_img_file *decomp);

/**
 * Initialize a JPEG compression object.
 *
 * @param[in] filename The filename to use.
 * @param[out] comp The image file object.
 * @returns True on success, false otherwise.
 */
bool init_jpeg_comp(const char *filename, infoto_img_file *comp);

#endif
