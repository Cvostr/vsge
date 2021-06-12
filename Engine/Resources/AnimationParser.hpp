#pragma once

#include <Graphics/Animation.hpp>
#include <Core/VarTypes/Base.hpp>
#include <Core/ByteSerialize.hpp>
#include <string>

namespace VSGE {
    class AnimationFileExport {
    private:
        Animation* _AnimationToExport;
    public:

        void writeChannel(ByteSerialize* serializer, uint32 index);
        void write(const std::string& output_file);

        AnimationFileExport(Animation* anim_ptr);
    };

    class ImportedAnimationFile {
    public:
        Animation* _Animation;

        void loadFromBuffer(byte* buffer, uint32 size);
        void loadFromFile(const std::string& file);

        ImportedAnimationFile();
        ~ImportedAnimationFile();
    };
}