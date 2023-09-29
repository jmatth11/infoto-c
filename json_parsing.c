#include <string.h>

#include "deps/frozen/frozen.h"

#include "img_utils.h"
#include "json_parsing.h"

/* Main JSON file format */
static const char *INFOTO_JSON_FORMAT = "{"
                                        " metadata:[%M],"
                                        " font:%M,"
                                        " background:%M,"
                                        " target:%Q"
                                        "}";

/* Metadata info JSON format */
static const char *METADATA_JSON_FORMAT =
    "{"
    " name:%s,"
    /* Quoted string, because they can be empty */
    " prefix:%Q,"
    /* Quoted string, because they can be empty */
    " postfix:%Q"
    "}";

/* Font info JSON format */
static const char *FONT_JSON_FORMAT = "{"
                                      " point:%d,"
                                      " ttf_file:%Q,"
                                      " color:%s"
                                      "}";

/* Background info JSON format */
static const char *BACKGROUND_JSON_FORMAT = "{"
                                            " color:%s,"
                                            " pixels:%d"
                                            "}";

/**
 * Callback function for parsing metadata list in json.
 */
static void parse_metadata_list(const char *str, int len, void *user_data) {
  config *out_cfg = (config *)user_data;
  int i;
  struct json_token t;
  // iterate through all elements in array
  for (i = 0; json_scanf_array_elem(str, len, "", i, &t) > 0; ++i) {
    metadata_info info;
    // ensure struct is empty
    memset(&info, 0, sizeof(info));
    // prefix and postfix can be empty string
    // so pull them out as Quoted strings
    char *prefix = NULL, *postfix = NULL;
    // scan out values
    int result = json_scanf(t.ptr, t.len, METADATA_JSON_FORMAT, &info.name,
                            &prefix, &postfix);
    if (result < 0) {
      fprintf(stderr, "json scanf error: parse_metadata_list; for loop {%d}\n",
              i);
      continue;
    }
    // we only allow 10 characters for each
    strncpy(info.prefix, prefix, CONFIG_INFO_FIX_LEN);
    strncpy(info.postfix, postfix, CONFIG_INFO_FIX_LEN);
    // free them
    free(prefix);
    free(postfix);
    // insert item onto array
    if (!insert_metadata_array(&out_cfg->metadata, info)) {
      fprintf(stderr, "reallocating array failed\n");
    }
  }
}

/**
 * Callback function for parsing font info in json.
 */
static void parse_font_info(const char *str, int len, void *user_data) {
  config *out_cfg = (config *)user_data;
  font_info info;
  char font_color[CONFIG_COLOR_LEN];
  if (json_scanf(str, len, FONT_JSON_FORMAT, &info.point, &info.ttf_file,
                 &font_color) < 0) {
    fprintf(stderr, "json scanf error: parse_font_info\n");
    return;
  }
  info.color = infoto_get_background_color_from_string(font_color);
  out_cfg->font = info;
}

/**
 * Callback function for parsing background info in json.
 */
static void parse_background_info(const char *str, int len, void *user_data) {
  config *out_cfg = (config *)user_data;
  background_info info;
  char color_text[20];
  if (json_scanf(str, len, BACKGROUND_JSON_FORMAT, &color_text, &info.pixels) <
      0) {
    fprintf(stderr, "json scanf error: parse_background_info\n");
    return;
  }
  info.color = infoto_get_background_color_from_string(color_text);
  out_cfg->background = info;
}

/**
 * Populate config object with JSON file.
 *
 * @param file_name The JSON file name path.
 * @param cfg The struct config to populate.
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum config_from_json_file(const char *file_name, config *cfg) {
  // read in file
  char *json_data = json_fread(file_name);
  if (json_data == NULL) {
    fprintf(stderr, "error reading json file: %s\n", file_name);
    return INFOTO_ERR_OPEN_FILE;
  }
  // initialize config
  infoto_init_config(cfg);
  // scan json file
  // %M format is (callback, user_data)
  if (json_scanf(json_data, strlen(json_data), INFOTO_JSON_FORMAT,
                 &parse_metadata_list, cfg, &parse_font_info, cfg,
                 &parse_background_info, cfg, &cfg->target) <= 0) {
    fprintf(stderr, "json scanf error: config_from_json_file.\n");
    return INFOTO_ERR_JSON_GENERIC;
  };
  // free buffer from file
  free(json_data);
  return INFOTO_SUCCESS;
}
