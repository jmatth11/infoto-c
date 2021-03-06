#include "exif.h"
#include "config.h"
#include "str_utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libexif/exif-content.h>
#include <libexif/exif-data.h>
#include <libexif/exif-format.h>
#include <libexif/exif-tag.h>
#include <libexif/exif-utils.h>

#define FORMATTED_STRING_LEN (CONFIG_INFO_FIX_LEN * 2)

/**
 * Get the value from the entry object into a string.
 *
 * @param[in] entry The ExifEntry object
 * @param[in] order The ExifData byte order
 * @param[out] out The char array to populate
 */
static void get_entry_value_str(ExifEntry *entry, ExifByteOrder order,
                                char *out) {
  switch (entry->format) {
  case EXIF_FORMAT_SHORT: {
    ExifShort tmp = exif_get_short(entry->data, order);
    sprintf(out, "%d", tmp);
  } break;
  case EXIF_FORMAT_SSHORT: {
    ExifSShort tmp = exif_get_sshort(entry->data, order);
    sprintf(out, "%d", tmp);
  } break;
  case EXIF_FORMAT_LONG: {
    ExifLong tmp = exif_get_long(entry->data, order);
    sprintf(out, "%d", tmp);
  } break;
  case EXIF_FORMAT_SLONG: {
    ExifSLong tmp = exif_get_slong(entry->data, order);
    sprintf(out, "%d", tmp);
  } break;
  case EXIF_FORMAT_RATIONAL: {
    ExifRational tmp = exif_get_rational(entry->data, order);
    // TODO may need to handle special cases, not sure yet
    if (tmp.numerator > 1) {
      sprintf(out, "%.1f", (double)tmp.numerator / (double)tmp.denominator);
    } else {
      sprintf(out, "%d/%d", tmp.numerator, tmp.denominator);
    }
  } break;
  case EXIF_FORMAT_SRATIONAL: {
    ExifSRational tmp = exif_get_srational(entry->data, order);
    // TODO may need to handle special cases, not sure yet
    if (tmp.numerator > 1) {
      sprintf(out, "%.1f", (double)tmp.numerator / (double)tmp.denominator);
    } else {
      sprintf(out, "%d/%d", tmp.numerator, tmp.denominator);
    }
  } break;
  default: {
    // handles EXIF_FORMAT_{ASCII|BYTE|SBYTE}
    sprintf(out, "%s", entry->data);
  } break;
  }
}

/**
 * Get EXIF data object from file if file exists.
 *
 * @param[in] file_name The file to access
 * @param[out] exif The EXIF data object to populate
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
static infoto_error_enum get_exif_data(const char *file_name, ExifData **exif) {
  // check the access of the file
  if (access(file_name, F_OK) != 0) {
    fprintf(stderr, "file is not accessible: %s\n", file_name);
    return INFOTO_ERR_NO_FILE_ACCESS;
  }
  // read out EXIF data
  *exif = exif_data_new_from_file(file_name);
  if (exif == NULL) {
    fprintf(stderr, "could not read exif data for file: %s\n", file_name);
    return INFOTO_ERR_EXIF_READ;
  }
  return INFOTO_SUCCESS;
}

static char *get_info_text_buffer(const ExifEntry *entry, const char *value,
                                  const metadata_info mi) {
  const int buffer_size = (FORMATTED_STRING_LEN + entry->size);
  const size_t char_size = sizeof(char);
  char *buffer = (char *)malloc((char_size * buffer_size) + char_size);
  if (buffer == NULL) {
    fprintf(stderr, "malloc of buffer failed.\n");
    return NULL;
  }
  // get formated string
  // buffer_N is number of characters in buffer, not including null character
  const int buffer_N = snprintf(buffer, buffer_size + 1, "%s%s%s", mi.prefix, value, mi.postfix);
  if (buffer_N < 0) {
    fprintf(stderr, "sprintf failed\n");
    return NULL;
  }
  // force uppercase
  for (int j = 0; j <= buffer_N; ++j) {
    buffer[j] = toupper(buffer[j]);
  }
  // ensure null terminated
  buffer[buffer_N + 1] = '\0';
  return buffer;
}

/**
 * Read EXIF data from JPEG file.
 *
 * @param[in] image_name The image to read EXIF data from.
 * @param[in] metadata The array of metadata info.
 * @param[out] output An info text buffer object
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum infoto_read_exif_data(const char *image_name,
                                        const metadata_array *metadata,
                                        info_text *output) {
  ExifData *exif = NULL;
  infoto_error_enum err_code = get_exif_data(image_name, &exif);
  if (err_code != INFOTO_SUCCESS) {
    return err_code;
  }
  ExifByteOrder byte_order = exif_data_get_byte_order(exif);
  // have a reusable value buffer
  char *value = NULL;
  size_t value_len = 0;
  // initialize buffers
  for (int i = 0; i < metadata->len; ++i) {
    // get metadata info object
    metadata_info mi;
    get_metadata_array(metadata, i, &mi);
    // grab the tag name
    ExifTag tag = exif_tag_from_name(mi.name);
    if (tag == 0) {
      fprintf(stderr, "tag not found. searching for: %s\n", mi.name);
      continue;
    }
    // grab the entry
    // this object is owned by the EXIF data object, do not unref
    ExifEntry *entry = exif_data_get_entry(exif, tag);
    if (entry == NULL) {
      fprintf(stderr, "failed to get %s entry\n", mi.name);
      err_code = INFOTO_ERR_EXIF_DATA;
      break;
    }
    // allocate more memory for our buffer if it's not big enough
    if (value_len <= entry->size) {
      // value_len is size + sizeof(char) for null character
      value_len = infoto_inc_string_size(&value, entry->size);
      if (value_len == -1) {
        fprintf(stderr, "inc_string_size failed.\n");
        err_code = INFOTO_ERR_INC_STR_SIZE;
        break;
      }
    }
    // clear out value buffer
    memset(value, 0, value_len - 1);
    // get entry value
    get_entry_value_str(entry, byte_order, value);
    // get buffer for info text
    char *buffer = get_info_text_buffer(entry, value, mi);
    if (buffer == NULL) {
      fprintf(stderr, "info text buffer failed.\n");
      err_code = INFOTO_ERR_INFO_TEXT_BUFF;
      break;
    }
    output->buffer[i] = buffer;
  }
  // free value buffer
  free(value);
  return err_code;
}

/**
 * Simple function to read EXIF data.
 * Used with exif_content_foreach_entry callback function.
 *
 * @param[in] entry The ExifEntry object.
 * @param[in, out] user_data The user data object passed in.
 *   In this case an infoto_exif_data_array.
 */
void read_exif_data(ExifEntry *entry, void *user_data) {
  infoto_exif_tag_info_array *data_arr =
      (infoto_exif_tag_info_array *)user_data;
  infoto_exif_tag_info exif_data;
  exif_data.value = entry->tag;
  exif_data.name =
      exif_tag_get_name_in_ifd(entry->tag, exif_content_get_ifd(entry->parent));
  insert_infoto_exif_tag_info_array(data_arr, exif_data);
}

/**
 * Read all EXIF tag names the given image has.
 *
 * @param[in] image_name The image to read EXIF tags from.
 * @param[in,out] info_arr The EXIF tag info array to populate.
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum
infoto_read_all_exif_tags(const char *image_name,
                          infoto_exif_tag_info_array *info_arr) {
  ExifData *exif = NULL;
  infoto_error_enum err_code = get_exif_data(image_name, &exif);
  if (err_code != INFOTO_SUCCESS) {
    return err_code;
  }
  init_infoto_exif_tag_info_array(info_arr, 1);
  for (int i = EXIF_IFD_0; i < EXIF_IFD_COUNT; ++i) {
    exif_content_foreach_entry(exif->ifd[i], read_exif_data, info_arr);
  }
  return INFOTO_SUCCESS;
}
