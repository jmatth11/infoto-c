#ifndef INFOTO_JSON_PARSING_H
#define INFOTO_JSON_PARSING_H

#include "config.h"

/**
 * Populate config object with JSON file.
 *
 * @param file_name The JSON file name path.
 * @param cfg The struct config to populate.
 * @return True if successful, False otherwise.
 */
bool config_from_json_file(const char *file_name, struct config *cfg);

#endif
