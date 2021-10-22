#ifndef INFOTO_TTF_UTIL
#define INFOTO_TTF_UTIL

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>

#include <stdbool.h>

/**
 * Structure for handling TTF library specific functionality.
 */
typedef struct infoto_font_handler infoto_font_handler;

/**
 * Structure to hold glyph info about a string.
 */
typedef struct infoto_glyph_str infoto_glyph_str;

/**
 * Initialize font handler structure for TTF fonts.
 *
 * @param[out] handler The infoto_font_handler to initialize.
 * @returns True if successful, false otherwise.
 */
bool infoto_font_handler_init(infoto_font_handler *handler);

/**
 * Load TTF font file with the given size.
 *
 * @param[out] handler The infoto_font_handler to load the TTF file into.
 * @param[in] ttf_file The TTF file to load.
 * @param[in] size The font size.
 * @returns True if successful, false otherwise.
 */
bool infoto_font_handler_load_font(infoto_font_handler *handler,
                                   const char *ttf_file, int size);

/**
 * Free all internal objects in infoto_font_handler.
 *
 * @param[out] handler The infoto_font_handler to free.
 */
void infoto_font_handler_free(infoto_font_handler *handler);

/**
 * Initialize an infoto_glyph_str structure.
 *
 * @param[out] str The infoto_glyph_str to initialize.
 * @returns True if successful, false otherwise.
 */
bool infoto_glyph_str_init(infoto_glyph_str **str);

/**
 * Get the width of the string of glyphs.
 *
 * @param[in] str The infoto_glyph_str to get the width of.
 * @returns The width of the string.
 */
int infoto_glyph_str_get_width(const infoto_glyph_str *str);

/**
 * Get the height of the string of glyphs.
 *
 * @param[in] str The infoto_glyph_str to get the height of.
 * @returns The height of the string.
 */
int infoto_glyph_str_get_height(const infoto_glyph_str *str);

/**
 * Add glyph to the glyph string.
 *
 * @param[out] str The infoto_glyph_str to update.
 * @param[in] glyph The glyph to add.
 * @returns True if successful, false otherwise.
 */
bool infoto_glyph_str_add(infoto_glyph_str *str, FT_Glyph glyph);

/**
 * Free glyph str.
 *
 * @param[out] str The infoto_glyph_str to free.
 */
void infoto_glyph_str_free(infoto_glyph_str *str);

/**
 * Create a glyph string with the given text.
 *
 * @param[in] handler The infoto font handler for TTF font info.
 * @param[out] glyph_str The infoto_glyph_str to populate.
 * @param[in] text The text to generate glyphs from.
 * @returns True if successful, false otherwise.
 */
bool create_glyph_str_from_text(infoto_font_handler *handler,
                                infoto_glyph_str *glyph_str, const char *text);

#endif
