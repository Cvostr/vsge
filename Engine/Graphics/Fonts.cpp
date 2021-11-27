#include "Fonts.hpp"
#include <iostream>
#include <algorithm>
#include <Core/FileLoader.hpp>
#include <Engine/Application.hpp>

using namespace VSGE;

GlyphManager glyphs;
GlyphManager* GlyphManager::_this = nullptr;

GlyphManager::GlyphManager() {
    _this = this;
    if (FT_Init_FreeType(&this->mFtlib))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
}

FT_Library GlyphManager::getFreetypeLibraryInstance() {
    return this->mFtlib;
}

void GlyphManager::AddFontContainer(GlyphFontContainer* ptr) {
    this->_fonts.push_back(ptr);
}

void GlyphManager::AddFontContainer(const std::string& file_path, const std::string& name) {
    if (Application::Get()->GetDescription().headless)
        return;

    byte* data = nullptr;
    uint32 size = 0;
    bool result = LoadFile(file_path, (char**)&data, &size);
    if (result) 
    {
        GlyphFontContainer* container = new GlyphFontContainer(data, size, 64);
        container->SetName(name);
        AddFontContainer(container);
    }
}

GlyphFontContainer* GlyphManager::GetFontByName(const std::string& name) {
    for (auto font : _fonts) {
        if (font->GetName() == name)
            return font;
    }
    return nullptr;
}

GlyphFontContainer::GlyphFontContainer(byte* data, uint32 bsize, uint32 size) {
    //Load font by freetype
    FT_New_Memory_Face(GlyphManager::Get()->getFreetypeLibraryInstance(), data, bsize, 0, &_ft_font);
    FT_Set_Pixel_Sizes(_ft_font, 0, size);
    _glyph_texture = CreateTexture();
    _font_size = size;
    //Allocate temporary buffer
    _glyph_texture_buffer = new byte[texSize * texSize];
    memset(_glyph_texture_buffer, 0, texSize * texSize);

    uint32 WorkX = 0;
    uint32 WorkY = 0;
    uint32 MaxY = 0;
    //Fill arrays and texture buffer
    loadGlyphs(WorkX, WorkY, MaxY);
    //Create texture from buffer
    _glyph_texture->Create(texSize, texSize, TextureFormat::FORMAT_R);
    _glyph_texture->AddMipLevel(_glyph_texture_buffer, texSize * texSize, texSize, texSize, 0, 0);
    _glyph_texture->SetReadyToUseInShaders();
    //Delete temporary buffer
    delete[] _glyph_texture_buffer;
}

GlyphFontContainer::~GlyphFontContainer() {
    FT_Done_Face(this->_ft_font);

    std::for_each(_characters.begin(), _characters.end(),
        [](std::pair<uint32, CharacterGlyph*> element) {
            // Accessing VALUE from element.
            delete element.second;
        });
}

void GlyphFontContainer::loadGlyphs(uint32& WorkX,
    uint32& WorkY,
    uint32& MaxY) {
    //Common ASCII symbols
    for (uint32 i = 0; i < 255; i++) {
        loadGlyph(i, WorkX, WorkY, MaxY);
    }
    _characters[32]->mGlyphSize.x = 20;
    //Cyrillic Russian letters
    for (uint32 i = 1040; i < 1106; i++) {
        loadGlyph(i, WorkX, WorkY, MaxY);
    }
    //Additional Cyrillic Ukrainian letters
    loadGlyph(1168, WorkX, WorkY, MaxY);
    loadGlyph(1169, WorkX, WorkY, MaxY);
    loadGlyph(1031, WorkX, WorkY, MaxY);
    loadGlyph(1111, WorkX, WorkY, MaxY);
    loadGlyph(1028, WorkX, WorkY, MaxY);
    loadGlyph(1108, WorkX, WorkY, MaxY);
    loadGlyph(1110, WorkX, WorkY, MaxY);
    loadGlyph(1030, WorkX, WorkY, MaxY);
    //symbol ¹
    loadGlyph(8470, WorkX, WorkY, MaxY);
}

void GlyphFontContainer::loadGlyph(uint32 index,
    uint32& WorkX,
    uint32& WorkY,
    uint32& MaxY) {
    //use freetype to load char
    FT_Load_Char(this->_ft_font, static_cast<FT_ULong>(index), FT_LOAD_RENDER);
    //Allocating new Character Glyph
    CharacterGlyph* character = new CharacterGlyph;
    character->mGlyphSize.x = static_cast<float>(_ft_font->glyph->bitmap.width);
    character->mGlyphSize.y = static_cast<float>(_ft_font->glyph->bitmap.rows);

    character->mGlyphAdvance.x = static_cast<float>(_ft_font->glyph->advance.x);
    character->mGlyphAdvance.y = static_cast<float>(_ft_font->glyph->advance.y);

    character->mGlyphBearing.x = static_cast<float>(_ft_font->glyph->bitmap_left);
    character->mGlyphBearing.y = static_cast<float>(_ft_font->glyph->bitmap_top);
    //This code adds glyph to general font texture
    if (((texSize - WorkX) < character->mGlyphSize.x)) {
        WorkX = 0;
        WorkY += MaxY;
        MaxY = 0;
    }
    if (MaxY < character->mGlyphSize.y)
        MaxY = static_cast<uint32>(character->mGlyphSize.y);

    character->mGlyphTextureStart = Vec2(static_cast<float>(WorkX), static_cast<float>(WorkY));

    for (uint32 _y = 0; _y < character->mGlyphSize.y; _y++) {
        uint32 Ycoord = (WorkY + _y) * texSize;
        for (uint32 _x = 0; _x < character->mGlyphSize.x; _x++) {
            uint32 Xcoord = Ycoord + WorkX + _x;
            _glyph_texture_buffer[Xcoord] = _ft_font->glyph->bitmap.buffer[(int)(_y * character->mGlyphSize.x + _x)];
        }
    }
    WorkX += static_cast<uint32>(character->mGlyphSize.x) + 1;

    _characters.insert(std::pair<uint32, CharacterGlyph*>(index, character));
}

void GlyphFontContainer::SetName(const std::string& name) {
    _name = name;
}
const std::string& GlyphFontContainer::GetName() {
    return _name;
}
Texture* GlyphFontContainer::GetTexture() {
    return _glyph_texture;
}
CharacterGlyph* GlyphFontContainer::GetGlyph(uint32 character) {
    return _characters.at(static_cast<uint32>(character));
}
Vec2i GlyphFontContainer::GetSizeOfString(uint32* str, uint32 strlen) { 
    Vec2i result = 0;
    for (uint32 i = 0; i < strlen; i++) {
        CharacterGlyph* glyph = GetGlyph(str[i]);
        result.x += glyph->mGlyphSize.x;
        if (result.y < glyph->mGlyphSize.y)
            result.y = glyph->mGlyphSize.x;
    }
    return result;
}