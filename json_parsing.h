#ifndef INFOTO_JSON_PARSING_H
#define INFOTO_JSON_PARSING_H

#include "config.h"
#include "error_codes.h"

/**
 * Populate config object with JSON file.
 *
 * @param file_name The JSON file name path.
 * @param cfg The struct config to populate.
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum config_from_json_file(const char *file_name, config *cfg);

#endif
