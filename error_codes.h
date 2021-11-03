#ifndef INFOTO_ERROR_REPORT_H
#define INFOTO_ERROR_REPORT_H

/**
 * List of error codes within infoto.
 */
typedef enum {
  INFOTO_SUCCESS = 0,
  INFOTO_ERR_NO_FILE_ACCESS,
  INFOTO_ERR_JSON_GENERIC,
  INFOTO_ERR_NULL,
  INFOTO_ERR_EXIF_READ,
  INFOTO_ERR_MALLOC,
  INFOTO_ERR_INFO_TEXT_BUFF,
  INFOTO_ERR_EXIF_DATA,
  INFOTO_ERR_INC_STR_SIZE,
  INFOTO_ERR_TTF_WRONG_FORMAT,
  INFOTO_ERR_OPEN_FILE,
  INFOTO_ERR_IMG_READ,
  INFOTO_ERR_IMG_WRITER,
  INFOTO_ERR_JPEG_HANDLER,
  INFOTO_ERR_TTF_INIT,
  INFOTO_ERR_TTF_UNKNOWN_FILE,
  INFOTO_ERR_TTF_GENERIC,
  INFOTO_ERR_TTF_PIXEL_SIZE,
  INFOTO_ERR_TTF_LOAD_CHAR,
  INFOTO_ERR_TTF_GET_GLYPH,
  INFOTO_ERR_GLYPH_STR_INIT,
  INFOTO_ERR_GLYPH_STR_ADD
} infoto_error_enum;

/**
 * Get the string representation of the given error code.
 *
 * @param[in] code The infoto error code.
 * @returns String representation of the error code.
 */
const char *infoto_err_code_to_str(const infoto_error_enum code);

#endif
