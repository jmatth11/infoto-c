#include "exif.h"
#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libexif/exif-content.h>
#include <libexif/exif-data.h>
#include <libexif/exif-tag.h>

#define FORMATTED_STRING_LEN (CONFIG_INFO_FIX_LEN * 2)

int inc_string_size(char **str, size_t size) {
  const size_t char_size = sizeof(char);
  // extra + char_size for null terminator
  const size_t N = (char_size * size) + char_size;
  char *tmp = NULL;
  if (*str == NULL) {
    tmp = (char *)malloc(N);
  } else {
    tmp = (char *)realloc(*str, N);
  }
  if (tmp == NULL)
    return -1;
  tmp[N - 1] = '\0';
  *str = tmp;
  return N;
}

bool read_exif_data(config *cfg, info_text *output) {
  const char *file_name = cfg->img;
  // check the access of the file
  if (access(file_name, F_OK) != 0) {
    printf("file is not accessible: %s\n", file_name);
    return false;
  }
  // read out EXIF data
  ExifData *exif = exif_data_new_from_file(file_name);
  if (exif == NULL) {
    printf("could not read exif data for file: %s\n", file_name);
    return false;
  }
  // have a reusable value buffer
  char *value = NULL;
  size_t value_len = 0;
  // initialize buffers
  for (int i = 0; i < cfg->metadata.len; ++i) {
    // get metadata info object
    metadata_info mi;
    get_metadata_array(&cfg->metadata, i, &mi);
    // grab the tag name
    ExifTag tag = exif_tag_from_name(mi.name);
    if (tag == 0) {
      printf("tag not found. searching for: %s\n", mi.name);
      continue;
    }
    // grab the entry
    // this object is owned by the EXIF data object, do not unref
    ExifEntry *entry = exif_data_get_entry(exif, tag);
    if (entry == NULL) {
      printf("failed to get %s entry\n", mi.name);
      return false;
    }
    if (value_len <= entry->size) {
      // value_len is size + sizeof(char) for null character
      value_len = inc_string_size(&value, entry->size);
      if (value_len == -1) {
        printf("inc_string_size failed.\n");
        return false;
      }
    }
    // value_len = (sizeof(char) * entry->size) + sizeof(char);
    // char *value = (char *)malloc(value_len);
    // clear out char array
    // value[value_len] = '\0';
    memset(value, 0, value_len);
    printf("entry->size: %u\n", entry->size);
    printf("value_len: %ld\n", value_len);
    // get the human readable value out
    // value is utf-8 encoded
    exif_entry_get_value(entry, value, entry->size);
    const int buffer_size = (FORMATTED_STRING_LEN + entry->size);
    const size_t char_size = sizeof(char);
    char *buffer = (char *)malloc((char_size * buffer_size) + char_size);
    if (buffer == NULL) {
      printf("malloc of buffer failed.\n");
      return false;
    }
    // get formated string
    // buffer_N is number of characters in buffer, not including null character
    printf("prefix: \"%s\"\n", mi.prefix);
    printf("value: \"%s\"\n", value);
    printf("postfix: \"%s\"\n", mi.postfix);
    const int buffer_N =
        sprintf(buffer, "%s%s%s", mi.prefix, value, mi.postfix);
    if (buffer_N < 0) {
      printf("sprintf failed\n");
      return false;
    }
    // uppercase everything
    for (int j = 0; j <= buffer_N; ++j) {
      buffer[j] = toupper(buffer[j]);
    }
    // ensure null terminated
    buffer[buffer_N + 1] = '\0';
    output->buffer[i] = buffer;
  }
  free(value);
  return true;
}
