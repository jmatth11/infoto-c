#ifndef INFOTO_CONFIG_H
#define INFOTO_CONFIG_H

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
typedef struct {
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
 * Enumeration of background colors.
 */
typedef enum {
  BACKGROUND_BLACK,
  BACKGROUND_BLUE,
  BACKGROUND_GREEN,
  BACKGROUND_RED,
  BACKGROUND_WHITE
} background_color;

/**
 * structure defining font info.
 */
typedef struct {
  // font point size
  int point;
  // color value
  background_color color;
  // file name for TTF file
  char *ttf_file;
} font_info;

/**
 * structure defining background info
 */
typedef struct {
  // color value
  background_color color;
  // how many pixels wide to add to the image
  int pixels;
} background_info;

/**
 * Configuration object to handle infoto logic
 */
typedef struct {
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
void infoto_init_config(config *cfg);

/**
 * Free configuration struct.
 *
 * @param cfg A config struct
 */
void infoto_free_config(config *cfg);

/**
 * Print the config structure.
 *
 * @param cfg The config structure
 */
void infoto_print_config(const config *cfg);

#endif
