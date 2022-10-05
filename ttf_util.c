#include "ttf_util.h"
#include "deps/array_template/array_template.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// create functions for glyph string
generate_array_template(infoto_bitmap_glyphs, FT_Glyph);

/**
 * Structure for handling TTF library specific functionality.
 */
struct infoto_font_library {
  FT_Library library;
};

/**
 * Structure for handling TTF library specific functionality.
 */
// struct infoto_font_handler {
//  FT_Library library;
//  FT_Face face;
//};

/**
 * Structure to hold glyph info about a string.
 */
struct infoto_glyph_str {
  infoto_bitmap_glyphs_array glyphs;
};

/**
 * Initialize font library structure for TTF fonts.
 *
 * @param[out] library The infoto_font_library to initialize.
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum
infoto_font_library_init(struct infoto_font_library **library) {
  struct infoto_font_library *local =
      (struct infoto_font_library *)malloc(sizeof(struct infoto_font_library));

  int error = FT_Init_FreeType(&local->library);
  if (error) {
    free(local);
    fprintf(stderr, "failed to initialize free type library.\n");
    return INFOTO_ERR_TTF_INIT;
  }
  *library = local;
  return INFOTO_SUCCESS;
}

/**
 * Load TTF font file with the given size.
 *
 * @param[in] library The infoto_font_library to load the TTF file with.
 * @param[in] ttf_file The TTF file to load.
 * @param[in] size The font size.
 * @param[out] face The infoto_font_face to load the TTF into.
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum
infoto_font_library_load_font(struct infoto_font_library *library,
                              const char *ttf_file, int size,
                              struct infoto_font_face *face) {
  // read ttf font. 0 grabs the first font (some ttf files have multiple fonts)
  FT_Error error = FT_New_Face(library->library, ttf_file, 0, &face->face);
  if (error == FT_Err_Unknown_File_Format) {
    fprintf(stderr, "Font file given could not be opened and read: \"%s\"\n",
            ttf_file);
    return INFOTO_ERR_TTF_UNKNOWN_FILE;
  } else if (error) {
    fprintf(stderr, "font file failed to load with code: %s.\n",
            FT_Error_String(error));
    return INFOTO_ERR_TTF_GENERIC;
  }
  // set width and height. 0 width means height param is used for both.
  error = FT_Set_Pixel_Sizes(face->face, 0, size);
  if (error) {
    fprintf(stderr, "failed to set pixel size for font.\n");
    return INFOTO_ERR_TTF_PIXEL_SIZE;
  }
  return INFOTO_SUCCESS;
}

/**
 * Free the TTF font face.
 *
 * @param[out] face The infoto_font_face to free.
 */
void infoto_font_face_free(struct infoto_font_face *face) {
  FT_Done_Face(face->face);
}

/**
 * Free all internal objects in infoto_font_library.
 *
 * @param[out] library The infoto_font_library to free.
 */
void infoto_font_library_free(struct infoto_font_library **library) {
  FT_Done_FreeType((*library)->library);
  free(*library);
  *library = NULL;
}

/**
 * Initialize an infoto_glyph_str structure.
 *
 * @param[out] str The infoto_glyph_str to initialize.
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum infoto_glyph_str_init(struct infoto_glyph_str **str) {
  (*str) = (struct infoto_glyph_str *)malloc(sizeof(struct infoto_glyph_str));
  return init_infoto_bitmap_glyphs_array(&(*str)->glyphs, 1)
             ? INFOTO_SUCCESS
             : INFOTO_ERR_GLYPH_STR_INIT;
}

/**
 * Get the width of the string of glyphs.
 *
 * @param[in] str The infoto_glyph_str to get the width of.
 * @returns The width of the string, -1 if failure.
 */
int infoto_glyph_str_get_width(const struct infoto_glyph_str *str) {
  int width = 0;
  for (int i = 0; i < str->glyphs.len; ++i) {
    FT_Glyph out;
    get_infoto_bitmap_glyphs_array(&str->glyphs, i, &out);
    if (out == NULL) {
      return -1;
    }
    FT_BitmapGlyph tmp = (FT_BitmapGlyph)out;
    size_t glyph_width = tmp->bitmap.width;
    if (glyph_width == 0) {
      glyph_width = WHITE_SPACE_SIZE;
    }
    width += (glyph_width + KERN_SIZE);
  }
  return width;
}

/**
 * Get the height of the string of glyphs.
 *
 * @param[in] str The infoto_glyph_str to get the height of.
 * @returns The height of the string, -1 if failure.
 */
int infoto_glyph_str_get_height(const struct infoto_glyph_str *str) {
  int height = 0;
  for (int i = 0; i < str->glyphs.len; ++i) {
    FT_Glyph out;
    get_infoto_bitmap_glyphs_array(&str->glyphs, i, &out);
    if (out == NULL) {
      return -1;
    }
    FT_BitmapGlyph tmp = (FT_BitmapGlyph)out;
    size_t glyph_height = tmp->bitmap.rows;
    if (glyph_height > height) {
      height = glyph_height;
    }
  }
  return height;
}

/**
 * Get the len of the glyph str.
 *
 * @param[in] str The infoto_glyph_str.
 * @returns The number of glyphs.
 */
size_t infoto_glyph_str_len(const struct infoto_glyph_str *str) {
  return str->glyphs.len;
}

/**
 * Get the glyph at the given index.
 *
 * @params[in] str The infoto_glyph_str.
 * @params[in] idx The index.
 * @returns FT_Glyph object, NULL if index out of bounds.
 */
FT_Glyph infoto_glyph_str_get_glyph(const struct infoto_glyph_str *str,
                                    int idx) {
  if (idx < 0 || idx >= str->glyphs.len) {
    return NULL;
  }
  return str->glyphs.infoto_bitmap_glyphs_data[idx];
}

/**
 * Add glyph to the glyph string.
 *
 * @param[out] str The infoto_glyph_str to update.
 * @param[in] glyph The glyph to add.
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum infoto_glyph_str_add(struct infoto_glyph_str *str,
                                       FT_Glyph glyph) {
  return insert_infoto_bitmap_glyphs_array(&str->glyphs, glyph)
             ? INFOTO_SUCCESS
             : INFOTO_ERR_GLYPH_STR_ADD;
}

/**
 * Free glyph str.
 *
 * @param[out] str The infoto_glyph_str to free.
 */
void infoto_glyph_str_free(struct infoto_glyph_str *str) {
  for (int i = 0; i < str->glyphs.len; ++i) {
    FT_Glyph tmp;
    get_infoto_bitmap_glyphs_array(&str->glyphs, i, &tmp);
    FT_Done_Glyph(tmp);
  }
  free_infoto_bitmap_glyphs_array(&str->glyphs);
  free(str);
}

/**
 * Create a glyph string with the given text.
 *
 * @param[in] face The infoto font face for TTF font info.
 * @param[out] glyph_str The infoto_glyph_str to populate.
 * @param[in] text The text to generate glyphs from.
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum
infoto_create_glyph_str_from_text(struct infoto_font_face *face,
                                  struct infoto_glyph_str *glyph_str,
                                  const char *text) {
  if (text == NULL) {
    fprintf(stderr, "text was null.\n");
    return INFOTO_ERR_NULL;
  }
  int text_len = strlen(text);
  int error = 0;
  FT_GlyphSlot slot = face->face->glyph;
  for (int i = 0; i < text_len; ++i) {
    error = FT_Load_Char(face->face, text[i], FT_LOAD_RENDER);
    if (error) {
      fprintf(stderr, "error code %d for loading char: %c\n", error, text[i]);
      return INFOTO_ERR_TTF_LOAD_CHAR;
    }
    FT_Glyph tmp = NULL;
    error = FT_Get_Glyph(slot, &tmp);
    if (error == 0) {
      infoto_glyph_str_add(glyph_str, tmp);
    } else {
      fprintf(stderr, "could not get glyph from FT_GlyphSlot. error code = %d",
              error);
      return INFOTO_ERR_TTF_GET_GLYPH;
    }
  }
  return INFOTO_SUCCESS;
}
