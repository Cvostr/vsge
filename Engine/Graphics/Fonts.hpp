#pragma once

#include <map>
#include <string>
#include <vector>

#include <Core/VarTypes/Base.hpp>
#include <Math/Vec2.hpp>
#include <Math/Color.hpp>
#include "Texture.hpp"
#include <ft2build.h>

#include FT_FREETYPE_H
#define texSize 2048

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
        byte* mGlyphTextureBuffer;
        Texture* mGlyphTexture;

        FT_Face font;
        std::map<uint32, CharacterGlyph*> characters;
    public:
        GlyphFontContainer(byte* data, uint32 bsize, uint32 size);
        ~GlyphFontContainer();

        void loadGlyphs(uint32& WorkX,
            uint32& WorkY,
            uint32& MaxY);
        void loadGlyph(uint32 index,
            uint32& WorkX,
            uint32& WorkY,
            uint32& MaxY);

        void DrawChar(int _char, Vec2 pos, uint32* char_length, Color color);
        void DrawString(const char32_t* string, uint32 len, Vec2 pos, Color color = Color(255, 255, 255));
    };
    
    class GlyphManager {
    private:

        static GlyphManager* _this;

        FT_Library mFtlib;
        std::vector<GlyphFontContainer*> mFonts;
    public:

        static GlyphManager* Get() {
            return _this;
        }

        void addFontContainer(GlyphFontContainer* ptr); //add font container to vector

        FT_Library getFreetypeLibraryInstance();
        GlyphManager();
    };
    
}