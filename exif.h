#include "config.h"
#include "error_codes.h"
#include "info_text.h"

/**
 * Read EXIF data from JPEG file.
 *
 * @param cfg The config structure
 * @param output An info text buffer object
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum infoto_read_exif_data(config *cfg, info_text *output);
