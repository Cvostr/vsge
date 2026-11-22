#pragma once

#include <map>
#include <string>

#include <Core/VarTypes/Base.hpp>
#include <Math/Vec2.hpp>
#include <Math/Color.hpp>
#include "Texture.hpp"
#include <ft2build.h>

#include FT_FREETYPE_H

namespace VSGE {

    class CharacterGlyph {
    private:

    public:
        Vec2 mGlyphSize;
        Vec2 mGlyphBearing;
        Vec2 mGlyphAdvance;
        Vec2 mGlyphTextureStart;

        CharacterGlyph() {

        }
    };

    class GlyphFontContainer {
    private:
        std::string _name;
        Texture* _glyph_texture;

        FT_Face _ft_font;
        uint32 _font_size;

        std::map<uint32, CharacterGlyph*> _characters;

        void loadGlyphs(byte* glyphTextureBuffer, 
            uint32& WorkX,
            uint32& WorkY,
            uint32& MaxY);
        void loadGlyph(uint32 index,
            byte* glyphTextureBuffer,
            uint32& WorkX,
            uint32& WorkY,
            uint32& MaxY);
    public:
        GlyphFontContainer();
        ~GlyphFontContainer();

        /// <summary>
        /// Create from TTF data in memory
        /// </summary>
        /// <param name="data">TTF data</param>
        /// <param name="bsize">TTF data size</param>
        /// <param name="size">pixel height</param>
        /// <returns>true if success</returns>
        bool Create(byte* data, uint32 bsize, uint32 size);
        /// <summary>
        /// Set font name
        /// </summary>
        /// <param name="name"></param>
        void SetName(const std::string& name);
        /// <summary>
        /// Get font name
        /// </summary>
        /// <returns></returns>
        const std::string& GetName();
        /// <summary>
        /// Get font texture
        /// </summary>
        /// <returns></returns>
        Texture* GetTexture();
        /// <summary>
        /// Get glyph by its id
        /// </summary>
        /// <param name="character"></param>
        /// <returns></returns>
        CharacterGlyph* GetGlyph(uint32 character);
        /// <summary>
        /// Get size of string in pixels of this font
        /// </summary>
        /// <param name="str">- string</param>
        /// <param name="strlen">- length of string</param>
        /// <returns></returns>
        Vec2i GetSizeOfString(uint32* str, uint32 strlen);
        /// <summary>
        /// Get size of square font texture, created from font
        /// </summary>
        /// <returns></returns>
        uint32 GetTextureMapSize() const;
        /// <summary>
        /// Get created font pixel height
        /// </summary>
        /// <returns></returns>
        uint32 GetFontSize() const;
    };
}