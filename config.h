#ifndef INFOTO_CONFIG_H
#define INFOTO_CONFIG_H

#include <stdbool.h>
#include <stdlib.h>

#include "deps/array_template/array_template.h"

/* define char array lengths */

/* {post|pre}fix length */
#define CONFIG_INFO_FIX_LEN 10
/* EXIF Tag name length */
#define CONFIG_INFO_NAME_LEN 50
/* color value length */
#define CONFIG_COLOR_LEN 20

/**
 * structure defining metadata info
 */
typedef struct metadata_info {
  // prefix value
  char prefix[CONFIG_INFO_FIX_LEN];
  // postfix value
  char postfix[CONFIG_INFO_FIX_LEN];
  // EXIF tag name to grab
  char name[CONFIG_INFO_NAME_LEN];
} metadata_info;

/**
 * Generate array for metadata info.
 */
generate_array_template(metadata, metadata_info);

/**
 * structure defining font info.
 */
typedef struct font_info {
  // font point size
  int point;
  // y offset from the bottom of the image
  float y_offset_pct;
  // color value
  char color[CONFIG_COLOR_LEN];
  // file name for TTF file
  char *ttf_file;
} font_info;

/**
 * structure defining background info
 */
typedef struct background_info {
  // color value
  char color[CONFIG_COLOR_LEN];
  // how many pixels wide to add to the image
  int pixels;
} background_info;

/**
 * Configuration object to handle infoto logic
 */
typedef struct config {
  font_info font;
  background_info background;
  metadata_array metadata;
  char *img;
} config;

/**
 * Initialize configuration struct.
 *
 * @param cfg A config struct
 */
void init_config(config *cfg);

/**
 * Free configuration struct.
 *
 * @param cfg A config struct
 */
void free_config(config *cfg);

/**
 * Print the config structure.
 *
 * @param cfg The config structure
 */
void print_config(const config *cfg);

#endif
