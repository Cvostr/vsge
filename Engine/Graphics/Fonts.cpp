#include "Fonts.hpp"
#include <iostream>
#include <algorithm>

using namespace VSGE;

GlyphManager glyphs;
GlyphManager* GlyphManager::_this = nullptr;

GlyphManager::GlyphManager() {
    if (FT_Init_FreeType(&this->mFtlib))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
}

FT_Library GlyphManager::getFreetypeLibraryInstance() {
    return this->mFtlib;
}

void GlyphManager::addFontContainer(GlyphFontContainer* ptr) {
    this->mFonts.push_back(ptr);
}

GlyphFontContainer::GlyphFontContainer(byte* data, uint32 bsize, uint32 size) {
    //Load font by freetype
    FT_New_Memory_Face(GlyphManager::Get()->getFreetypeLibraryInstance(), data, bsize, 0, &this->font);
    FT_Set_Pixel_Sizes(this->font, 0, size);
    mGlyphTexture = CreateTexture();
    //Allocate temporary buffer
    mGlyphTextureBuffer = new byte[texSize * texSize];
    memset(mGlyphTextureBuffer, 0, texSize * texSize);

    uint32 WorkX = 0;
    uint32 WorkY = 0;
    uint32 MaxY = 0;
    //Fill arrays and texture buffer
    loadGlyphs(WorkX, WorkY, MaxY);
    //Create texture from buffer
    mGlyphTexture->Create(texSize, texSize, TextureFormat::FORMAT_R);
    mGlyphTexture->AddMipLevel(mGlyphTextureBuffer, texSize * texSize, texSize, texSize, 0);
    //Delete temporary buffer
    delete[] mGlyphTextureBuffer;
}

GlyphFontContainer::~GlyphFontContainer() {
    FT_Done_Face(this->font);

    std::for_each(characters.begin(), characters.end(),
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
    //Cyrillic Russian letters
    for (uint32 i = 1040; i < 1103; i++) {
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
}

void GlyphFontContainer::loadGlyph(uint32 index,
    uint32& WorkX,
    uint32& WorkY,
    uint32& MaxY) {
    //use freetype to load char
    FT_Load_Char(this->font, static_cast<FT_ULong>(index), FT_LOAD_RENDER);
    //Allocating new Character Glyph
    CharacterGlyph* character = new CharacterGlyph;
    character->mGlyphSize.x = static_cast<float>(font->glyph->bitmap.width);
    character->mGlyphSize.y = static_cast<float>(font->glyph->bitmap.rows);

    character->mGlyphAdvance.x = static_cast<float>(font->glyph->advance.x);
    character->mGlyphAdvance.y = static_cast<float>(font->glyph->advance.y);

    character->mGlyphBearing.x = static_cast<float>(font->glyph->bitmap_left);
    character->mGlyphBearing.y = static_cast<float>(font->glyph->bitmap_top);
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
            mGlyphTextureBuffer[Xcoord] = font->glyph->bitmap.buffer[(int)(_y * character->mGlyphSize.x + _x)];
        }
    }
    WorkX += static_cast<uint32>(character->mGlyphSize.x);

    this->characters.insert(std::pair<uint32, CharacterGlyph*>(index, character));
}

void GlyphFontContainer::DrawChar(int _char, Vec2 pos, uint32* char_length, Color color) {
    CharacterGlyph* glyph = this->characters.at(static_cast<uint32>(_char));
    *char_length = static_cast<uint32>(glyph->mGlyphBearing.x + glyph->mGlyphSize.x);

    //game_data->pipeline->renderGlyph(glyph, static_cast<int>(pos.x), static_cast<int>(pos.Y - (glyph->mGlyphSize.Y - glyph->mGlyphBearing.Y)), static_cast<int>(glyph->mGlyphSize.X), static_cast<int>(glyph->mGlyphSize.Y), color);
}
/*
void GlyphFontContainer::DrawString(const char32_t* string, unsigned int len, Vec2 pos, Color color) {
    mGlyphTexture->Use(0);
    unsigned int xpos_offset = static_cast<unsigned int>(pos.x);
    for (unsigned int i = 0; i < len; i++) {
        unsigned int char_len = 0;
        DrawChar(string[i], Vec2(static_cast<float>(xpos_offset), pos.y), &char_len, color);
        xpos_offset += char_len;
    }
}*/