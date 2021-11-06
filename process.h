#ifndef INFOTO_PROCESS
#define INFOTO_PROCESS

#include "config.h"
#include "error_codes.h"
#include "img_utils.h"
#include "str_utils.h"

/**
 * Process a bulk of images with the given background and font info.
 *
 * @param[in] handler The image handler.
 * @param[in] background The background info.
 * @param[in] font The font info.
 * @param[in] metadata The array of metadata info.
 * @param[in] imgs The array of image filenames.
 * @returns INFOTO_SUCCESS if successful, otherwise infoto_error_enum error.
 */
infoto_error_enum infoto_process_bulk(struct infoto_img_handler *handler,
                                      const background_info background,
                                      const font_info font,
                                      const metadata_array *metadata,
                                      const string_array *imgs);

#endif
