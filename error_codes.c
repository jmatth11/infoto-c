#ifndef INFOTO_ERROR_CODES
#define INFOTO_ERROR_CODES

#include "error_codes.h"

/**
 * Get the string representation of the given error code.
 *
 * @param[in] code The infoto error code.
 * @returns String representation of the error code.
 */
const char *infoto_err_code_to_str(const infoto_error_enum code) {
  char *result;
  switch (code) {
  case INFOTO_SUCCESS:
    result = "INFOTO_SUCCESS";
    break;
  case INFOTO_ERR_NO_FILE_ACCESS:
    result = "INFOTO_ERR_NO_FILE_ACCESS";
    break;
  case INFOTO_ERR_JSON_GENERIC:
    result = "INFOTO_ERR_JSON_GENERIC";
    break;
  case INFOTO_ERR_NULL:
    result = "INFOTO_ERR_NULL";
    break;
  case INFOTO_ERR_EXIF_READ:
    result = "INFOTO_ERR_EXIF_READ";
    break;
  case INFOTO_ERR_MALLOC:
    result = "INFOTO_ERR_MALLOC";
    break;
  case INFOTO_ERR_INFO_TEXT_BUFF:
    result = "INFOTO_ERR_INFO_TEXT_BUFF";
    break;
  case INFOTO_ERR_EXIF_DATA:
    result = "INFOTO_ERR_EXIF_DATA";
    break;
  case INFOTO_ERR_INC_STR_SIZE:
    result = "INFOTO_ERR_INC_STR_SIZE";
    break;
  case INFOTO_ERR_TTF_WRONG_FORMAT:
    result = "INFOTO_ERR_TTF_WRONG_FORMAT";
    break;
  case INFOTO_ERR_OPEN_FILE:
    result = "INFOTO_ERR_OPEN_FILE";
    break;
  case INFOTO_ERR_IMG_READ:
    result = "INFOTO_ERR_IMG_READ";
    break;
  case INFOTO_ERR_IMG_WRITER:
    result = "INFOTO_ERR_IMG_WRITER";
    break;
  case INFOTO_ERR_JPEG_HANDLER:
    result = "INFOTO_ERR_JPEG_HANDLER";
    break;
  case INFOTO_ERR_TTF_INIT:
    result = "INFOTO_ERR_TTF_INIT";
    break;
  case INFOTO_ERR_TTF_UNKNOWN_FILE:
    result = "INFOTO_ERR_TTF_UNKNOWN_FILE";
    break;
  case INFOTO_ERR_TTF_GENERIC:
    result = "INFOTO_ERR_TTF_GENERIC";
    break;
  case INFOTO_ERR_TTF_PIXEL_SIZE:
    result = "INFOTO_ERR_TTF_PIXEL_SIZE";
    break;
  case INFOTO_ERR_TTF_LOAD_CHAR:
    result = "INFOTO_ERR_TTF_LOAD_CHAR";
    break;
  case INFOTO_ERR_TTF_GET_GLYPH:
    result = "INFOTO_ERR_TTF_GET_GLYPH";
    break;
  case INFOTO_ERR_GLYPH_STR_INIT:
    result = "INFOTO_ERR_GLYPH_STR_INIT";
    break;
  case INFOTO_ERR_GLYPH_STR_ADD:
    result = "INFOTO_ERR_GLYPH_STR_ADD";
    break;
  }
  return result;
}

#endif
