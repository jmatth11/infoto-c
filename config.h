#ifndef INFOTO_CONFIG_H
#define INFOTO_CONFIG_H

#include <stdbool.h>
#include <stdlib.h>

struct metadata_info {
  int order;
  char prefix[10];
  char postfix[10];
  char name[50];
};

struct metadata_list {
  size_t len;
  size_t cap;
  struct metadata_info *metadata;
};

struct font_info {
  int point;
  float y_offset_pct;
  char color[20];
  char *ttf_file;
};

struct background_info {
  char color[20];
  int pixels;
};

/**
 * Configuration object to handle infoto logic
 */
struct config {
  struct font_info font;
  struct background_info background;
  struct metadata_list metadata;
  char *img;
};

/**
 * Initialize configuration struct.
 *
 * @param cfg A config struct
 */
void init_config(struct config *cfg);

/**
 * Free configuration struct.
 *
 * @param cfg A config struct
 */
void free_config(struct config *cfg);

/**
 * Print the config structure.
 *
 * @param cfg The config structure
 */
void print_config(const struct config *cfg);

/**
 * Initialize metadata list to a given size.
 *
 * @param list The metadata_list structure
 * @param N Size
 */
void init_metadata_list(struct metadata_list *list, size_t N);

/**
 * Insert metadata info structure into metadata list.
 *
 * @param list The metadata_list structure
 * @param info The metadata_info structure
 */
void insert_metadata_list(struct metadata_list *list,
                          struct metadata_info info);

/**
 * Get metadata info structure from metadata list given an index.
 *
 * @param list The metadata_list structure
 * @param index The index to access
 * @return The metadata_info at the index, NULL if it wasn't found
 */
const struct metadata_info *get_metadata_list(const struct metadata_list *list,
                                              size_t index);

/**
 * Free the metadata list.
 *
 * @param list The metadata_list structure
 */
void free_metadata_list(struct metadata_list *list);

#endif
