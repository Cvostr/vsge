#include <Graphics/FontManager.hpp>
#include <iostream>
#include <Core/FileLoader.hpp>

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
    this->m_fonts.push_back(ptr);
}

void GlyphManager::AddFontContainer(const std::string& filePath, const std::string& name) {
    byte* data = nullptr;
    uint32 size = 0;
    bool result = LoadFile(filePath, (char**)&data, &size);
    if (result)
    {
        GlyphFontContainer* container = new GlyphFontContainer(data, size, 64);
        container->SetName(name);
        AddFontContainer(container);
    }
}

GlyphFontContainer* GlyphManager::GetFontByName(const std::string& name) {
    for (auto font : m_fonts) {
        if (font->GetName() == name)
            return font;
    }
    return nullptr;
}