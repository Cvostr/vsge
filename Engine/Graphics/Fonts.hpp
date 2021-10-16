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
        std::string _name;
        byte* _glyph_texture_buffer;
        Texture* _glyph_texture;

        FT_Face _ft_font;
        uint32 _font_size;

        std::map<uint32, CharacterGlyph*> _characters;
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
        void SetName(const std::string& name);
        const std::string& GetName();
        Texture* GetTexture();
        CharacterGlyph* GetGlyph(uint32 character);
    };
    
    class GlyphManager {
    private:

        static GlyphManager* _this;

        FT_Library mFtlib;
        std::vector<GlyphFontContainer*> _fonts;
    public:

        static GlyphManager* Get() {
            return _this;
        }

        void AddFontContainer(const std::string& file_path, const std::string& name);
        void AddFontContainer(GlyphFontContainer* ptr); //add font container to vector

        GlyphFontContainer* GetFontByName(const std::string& name);

        FT_Library getFreetypeLibraryInstance();
        GlyphManager();
    };
    
}