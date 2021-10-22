#ifndef INFOTO_JPEG_HANDLER_H
#define INFOTO_JPEG_HANDLER_H

#include "img_utils.h"
#include "ttf_util.h"

/**
 * Initialize a infoto JPEG handler in the given img handler interface.
 *
 * @param[out] img_handler The image handler interface to populate.
 * @param[in] font_handler The font handler for the JPEG handler to reference.
 */
void infoto_jpeg_handler_init(infoto_img_handler *img_handler,
                              infoto_font_handler *font_handler);

/**
 * Free the internal JPEG handler.
 * This function does not free the font handler given at initialization.
 *
 * @param[out] img_handler The img handler to free.
 */
void infoto_jpeg_handler_free(infoto_img_handler *img_handler);

#endif
