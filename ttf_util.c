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
struct infoto_font_handler {
  FT_Library library;
  FT_Face face;
};

/**
 * Structure to hold glyph info about a string.
 */
struct infoto_glyph_str {
  infoto_bitmap_glyphs_array glyphs;
};

/**
 * Initialize font handler structure for TTF fonts.
 *
 * @param[out] handler The infoto_font_handler to initialize.
 * @returns True if successful, false otherwise.
 */
bool infoto_font_handler_init(struct infoto_font_handler *handler) {
  int error = FT_Init_FreeType(&handler->library);
  if (error) {
    fprintf(stderr, "failed to initialize free type library.\n");
    return false;
  }
  return true;
}

/**
 * Load TTF font file with the given size.
 *
 * @param[out] handler The infoto_font_handler to load the TTF file into.
 * @param[in] ttf_file The TTF file to load.
 * @param[in] size The font size.
 * @returns True if successful, false otherwise.
 */
bool infoto_font_handler_load_font(struct infoto_font_handler *handler,
                                   const char *ttf_file, int size) {
  // read ttf font. 0 grabs the first font (some ttf files have multiple fonts)
  int error = FT_New_Face(handler->library, ttf_file, 0, &handler->face);
  if (error == FT_Err_Unknown_File_Format) {
    fprintf(stderr, "Font file given could not be opened and read: \"%s\"\n",
            ttf_file);
    return false;
  } else if (error) {
    fprintf(stderr, "font file failed to load.\n");
    return false;
  }
  // set width and height. 0 width means height param is used for both.
  error = FT_Set_Pixel_Sizes(handler->face, 0, size);
  if (error) {
    fprintf(stderr, "failed to set pixel size for font.\n");
    return false;
  }
  return true;
}

/**
 * Free all internal objects in infoto_font_handler.
 *
 * @param[out] handler The infoto_font_handler to free.
 */
void infoto_font_handler_free(struct infoto_font_handler *handler) {
  FT_Done_Face(handler->face);
  FT_Done_FreeType(handler->library);
}

/**
 * Initialize an infoto_glyph_str structure.
 *
 * @param[out] str The infoto_glyph_str to initialize.
 * @returns True if successful, false otherwise.
 */
bool infoto_glyph_str_init(struct infoto_glyph_str **str) {
  (*str) = (struct infoto_glyph_str *)malloc(sizeof(struct infoto_glyph_str));
  return init_infoto_bitmap_glyphs_array(&(*str)->glyphs, 1);
}

/**
 * Get the width of the string of glyphs.
 *
 * @param[in] str The infoto_glyph_str to get the width of.
 * @returns The width of the string.
 */
int infoto_glyph_str_get_width(const struct infoto_glyph_str *str) {
  int width = 0;
  for (int i = 0; i < str->glyphs.len; ++i) {
    FT_Glyph out;
    get_infoto_bitmap_glyphs_array(&str->glyphs, i, &out);
    FT_BitmapGlyph tmp = (FT_BitmapGlyph)out;
    width += tmp->bitmap.width;
  }
  return width;
}

/**
 * Get the height of the string of glyphs.
 *
 * @param[in] str The infoto_glyph_str to get the height of.
 * @returns The height of the string.
 */
int infoto_glyph_str_get_height(const struct infoto_glyph_str *str) {
  int height = 0;
  if (str->glyphs.len > 0) {
    FT_Glyph out;
    get_infoto_bitmap_glyphs_array(&str->glyphs, 0, &out);
    FT_BitmapGlyph tmp = (FT_BitmapGlyph)out;
    height = tmp->bitmap.rows;
  }
  return height;
}

/**
 * Add glyph to the glyph string.
 *
 * @param[out] str The infoto_glyph_str to update.
 * @param[in] glyph The glyph to add.
 * @returns True if successful, false otherwise.
 */
bool infoto_glyph_str_add(struct infoto_glyph_str *str, FT_Glyph glyph) {
  return insert_infoto_bitmap_glyphs_array(&str->glyphs, glyph);
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
 * @param[in] handler The infoto font handler for TTF font info.
 * @param[out] glyph_str The infoto_glyph_str to populate.
 * @param[in] text The text to generate glyphs from.
 * @returns True if successful, false otherwise.
 */
bool create_glyph_str_from_text(struct infoto_font_handler *handler,
                                struct infoto_glyph_str *glyph_str,
                                const char *text) {
  int text_len = strlen(text);
  int error = 0;
  FT_GlyphSlot slot = handler->face->glyph;
  // TODO write out
  for (int i = 0; i < text_len; ++i) {
    error = FT_Load_Char(handler->face, text[i], FT_LOAD_RENDER);
    if (error) {
      fprintf(stderr, "error code %d for loading char: %c\n", error, text[i]);
      return false;
    }
    FT_Glyph tmp = NULL;
    error = FT_Get_Glyph(slot, &tmp);
    if (error == 0) {
      infoto_glyph_str_add(glyph_str, tmp);
    } else {
      fprintf(stderr, "could not get glyph from FT_GlyphSlot. error code = %d",
              error);
      return false;
    }

    // TODO write out slot->bitmap to buffer image.
    /**
     * draw_bitmap(&slot->bitmap,
     *           pen_x + slot->bitmap_left,
     *           pen_y - slot->bitmap_top);
     */
    // pen_x += slot->advance.x >> 6;
  }
  return true;
}
