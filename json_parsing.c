#include "json_parsing.h"
#include "deps/frozen/frozen.h"
#include <string.h>

/* Main JSON file format */
static const char *INFOTO_JSON_FORMAT = "{"
                                        " metadata:%M,"
                                        " font:%M,"
                                        " background:%M,"
                                        " image:%Q"
                                        "}";

/* Metadata info JSON format */
static const char *METADATA_JSON_FORMAT = "{"
                                          " name:%s,"
                                          " prefix:%s,"
                                          " postfix:%s,"
                                          " order:%d"
                                          "}";

/* Font info JSON format */
static const char *FONT_JSON_FORMAT = "{"
                                      " point:%d,"
                                      " ttf_file:%Q,"
                                      " color:%s,"
                                      " y_offset_pct:%f"
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
  struct config *out_cfg = (struct config *)user_data;
  int i;
  struct json_token t;
  for (i = 0; json_scanf_array_elem(str, len, "", i, &t) > 0; ++i) {
    struct metadata_info info;
    // ensure struct is empty
    memset(&info, 0, sizeof(info));
    if (json_scanf(t.ptr, t.len, METADATA_JSON_FORMAT, &info.name, &info.prefix,
                   &info.postfix, &info.order) < 0) {
      printf("json scanf error: parse_metadata_list; for loop {%d}\n", i);
      continue;
    }
    insert_metadata_list(&out_cfg->metadata, info);
  }
}

/**
 * Callback function for parsing font info in json.
 */
static void parse_font_info(const char *str, int len, void *user_data) {
  struct config *out_cfg = (struct config *)user_data;
  struct font_info info;
  char *filename = NULL;
  if (json_scanf(str, len, FONT_JSON_FORMAT, &info.point, &filename,
                 &info.color, &info.y_offset_pct) < 0) {
    printf("json scanf error: parse_font_info\n");
    return;
  }
  info.ttf_file = filename;
  out_cfg->font = info;
}

/**
 * Callback function for parsing background info in json.
 */
static void parse_background_info(const char *str, int len, void *user_data) {
  struct config *out_cfg = (struct config *)user_data;
  struct background_info info;
  if (json_scanf(str, len, BACKGROUND_JSON_FORMAT, info.color, &info.pixels) <
      0) {
    printf("json scanf error: parse_background_info\n");
    return;
  }
  out_cfg->background = info;
}

/**
 * Populate config object with JSON file.
 *
 * @param file_name The JSON file name path.
 * @param cfg The struct config to populate.
 * @return True if successful, False otherwise.
 */
bool config_from_json_file(const char *file_name, struct config *cfg) {
  printf("config_from_json_file\n");
  char *json_data = json_fread(file_name);
  if (json_data == NULL) {
    printf("error reading json file: %s\n", file_name);
    return false;
  }
  init_config(cfg);
  if (json_scanf(json_data, strlen(json_data), INFOTO_JSON_FORMAT,
                 &parse_metadata_list, cfg, &parse_font_info, cfg,
                 &parse_background_info, cfg, &cfg->img) <= 0) {
    printf("json scanf error: config_from_json_file.\n");
    return false;
  };
  free(json_data);
  return true;
}
