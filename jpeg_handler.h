#ifndef INFOTO_JPEG_HANDLER_H
#define INFOTO_JPEG_HANDLER_H

#include <stdbool.h>
#include <stdio.h>

#include "config.h"
#include "info_text.h"

/**
 * Add text and border to image, given config and info text.
 *
 * @param[in] cfg The config object
 * @param[in] info The info text object
 * @return True if successful, False otherwise
 */
bool add_text_to_img(const config *cfg, const info_text *info);

#endif
