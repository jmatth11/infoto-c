#include <stdbool.h>

#include "config.h"
#include "info_text.h"

/**
 * Read EXIF data from JPEG file.
 *
 * @param cfg The config structure
 * @param output An info text buffer object
 * @returns bool true for success, false otherwise
 */
bool infoto_read_exif_data(config *cfg, info_text *output);
