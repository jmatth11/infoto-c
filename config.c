#include "config.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void init_config(struct config *cfg) { init_metadata_list(&cfg->metadata, 0); }

void free_config(struct config *cfg) {
  free(cfg->img);
  free(cfg->font.ttf_file);
  free_metadata_list(&cfg->metadata);
}

void print_config(const struct config *cfg) {
  if (cfg == NULL) {
    printf("config structure was NULL for print\n");
    return;
  }
  printf("config structure: {\n");
  printf("img: \"%s\"\n", cfg->img);
  printf("font: {\n");
  if (cfg->font.ttf_file == NULL) {
    printf("config font ttf_file was null\n");
    return;
  }
  printf("\tttf_file: %s\n", cfg->font.ttf_file);
  printf("\tcolor: %s\n", cfg->font.color);
  printf("\tpoint: %d\n", cfg->font.point);
  printf("\ty_offset_pct: %f\n", cfg->font.y_offset_pct);
  printf("}\n");
  printf("background: {\n");
  printf("\tcolor: %s\n", cfg->background.color);
  printf("\tpixels: %d\n", cfg->background.pixels);
  printf("}\n");
  printf("metadata: [\n");
  for (int i = 0; i < cfg->metadata.len; ++i) {
    const struct metadata_info *info = get_metadata_list(&cfg->metadata, i);
    printf("\t{\n");
    printf("\t\tname: %s\n", info->name);
    printf("\t\tpostfix: %s\n", info->postfix);
    printf("\t\tprefix: %s\n", info->prefix);
    printf("\t\torder: %d\n", info->order);
    printf("\t{\n");
  }
  printf("]\n}\n");
}

void init_metadata_list(struct metadata_list *list, size_t N) {
  list->metadata =
      (struct metadata_info *)malloc(N * sizeof(struct metadata_info));
  list->len = 0;
  list->cap = N;
}

void insert_metadata_list(struct metadata_list *list,
                          struct metadata_info info) {
  if (list->len >= list->cap) {
    if (list->cap <= 0)
      list->cap = 1;
    list->cap += floor((float)list->cap * 1.3);
    list->metadata = (struct metadata_info *)realloc(list->metadata,
                                                     list->cap * sizeof(info));
  }
  list->metadata[list->len++] = info;
}

const struct metadata_info *get_metadata_list(const struct metadata_list *list,
                                              size_t index) {
  if (index >= list->len || index < 0) {
    return NULL;
  }
  return &list->metadata[index];
}

void free_metadata_list(struct metadata_list *list) {
  free(list->metadata);
  list->metadata = NULL;
  list->len = list->cap = 0;
}
