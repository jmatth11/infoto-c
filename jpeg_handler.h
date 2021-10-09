#ifndef INFOTO_JPEG_HANDLER_H
#define INFOTO_JPEG_HANDLER_H

#include <stdbool.h>
#include <stdio.h>

#include "config.h"
#include "info_text.h"

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
bool write_jpeg_image(const char *filename, const background_info background,
                      const info_text *info);

#endif
