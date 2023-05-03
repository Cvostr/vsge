#include "Fonts.hpp"
#include <vector>

namespace VSGE {
    class GlyphManager {
    private:

        static GlyphManager* _this;

        FT_Library mFtlib;
        std::vector<GlyphFontContainer*> m_fonts;
    public:

        static GlyphManager* Get() {
            return _this;
        }

        void AddFontContainer(const std::string& filePath, const std::string& name);
        void AddFontContainer(GlyphFontContainer* ptr); //add font container to vector

        GlyphFontContainer* GetFontByName(const std::string& name);

        FT_Library getFreetypeLibraryInstance();
        GlyphManager();
    };
}