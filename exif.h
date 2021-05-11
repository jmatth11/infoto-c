#include <stdbool.h>

#include "config.h"

/**
 * Read EXIF data from JPEG file.
 *
 * @param cfg The config structure
 * @param buffer An array of char[50]s
 * @returns bool true for success, false otherwise
 */
bool read_exif_data(struct config *cfg, char (*output)[50]);

