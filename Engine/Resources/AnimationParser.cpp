#include "AnimationParser.hpp"
#include <Core/ByteSolver.hpp>
#include <fstream>
#include "string.h"

using namespace VSGE;

ImportedAnimationFile::ImportedAnimationFile() {
    m_animation = nullptr;
}

ImportedAnimationFile::~ImportedAnimationFile() {
    SAFE_RELEASE(m_animation);
}

void ImportedAnimationFile::loadFromBuffer(byte* buffer, uint32 size) {
    ByteSolver solver(buffer, size);
    char prefix[6];
    prefix[5] = '\0';

    if (solver.ReadNextString() != "zs3manim") {
        //File's header is incorrect
        return;
    }
    //Allocate animation class
    m_animation = new Animation; //allocate animation
    //Read animation name
    m_animation->SetName(solver.ReadNextString());
    //Read Tick Per Second property
    m_animation->SetTPS(solver.GetValue<double>());
    //Read duration property
    m_animation->SetDuration(solver.GetValue<double>());
    //Read Channel amount
    m_animation->SetNumChannels(solver.GetValue<uint32>());
    //Allocate all channels
    m_animation->SetChannels(new AnimationChannel[m_animation->GetNumChannels()]);
    uint32 ch_i = 0;

    while (!solver.end()) {
        solver.Copy(prefix, 5);

        if (strcmp(prefix, "_CHAN") == 0) {
            //Read node, channel will work with
            //Allocate animation channel
            AnimationChannel* chan = &m_animation->GetChannels()[ch_i++];
            chan->SetBoneName(solver.ReadNextString());
            chan->anim_ptr = m_animation;

            chan->SetPositionKeysCount(solver.GetValue<uint32>());
            chan->SetScaleKeysCount(solver.GetValue<uint32>());
            chan->SetRotationKeysCount(solver.GetValue<uint32>());

            for (uint32 pos_i = 0; pos_i < chan->GetPosValues()->keysCount; pos_i++) {
                //reading pos vector
                chan->GetPosValues()->values[pos_i] = solver.GetValue<Vec3>();
                //Read position timings
                chan->GetPosValues()->times[pos_i] = solver.GetValue<double>();
            }
            for (uint32 sca_i = 0; sca_i < chan->GetScaleValues()->keysCount; sca_i++) {
                //reading scale vector
                chan->GetScaleValues()->values[sca_i] = solver.GetValue<Vec3>();
                //Read scale timings
                chan->GetScaleValues()->times[sca_i] = solver.GetValue<double>();
            }
            for (uint32 rot_i = 0; rot_i < chan->GetRotationValues()->keysCount; rot_i++) {
                //reading scale vector
                chan->GetRotationValues()->values[rot_i] = solver.GetValue<Quat>();
                //Read rotation timings
                chan->GetRotationValues()->times[rot_i] = solver.GetValue<double>();
            }
        }
    }
}

void ImportedAnimationFile::loadFromFile(const std::string& file) {
    std::ifstream stream;
    stream.open(file, std::iostream::binary | std::iostream::ate);
    uint32 zs3m_size = static_cast<uint32>(stream.tellg());
    stream.seekg(0, std::ifstream::beg);
    char* file_buffer = new char[zs3m_size];
    stream.read(file_buffer, zs3m_size);

    loadFromBuffer((byte*)file_buffer, zs3m_size);

    delete[] file_buffer;
}


AnimationFileExport::AnimationFileExport(Animation* anim_ptr) {
    this->_AnimationToExport = anim_ptr;
}

void AnimationFileExport::writeChannel(ByteSerialize* serializer, uint32 index) {
    AnimationChannel* channel = &_AnimationToExport->GetChannels()[index];
    //Write channel prefix and its name
    serializer->WriteBytes("_CHAN", 5);
    serializer->Serialize(channel->GetBoneName());

    serializer->Serialize(channel->GetPosValues()->keysCount);
    serializer->Serialize(channel->GetScaleValues()->keysCount);
    serializer->Serialize(channel->GetRotationValues()->keysCount);

    for (uint32 pos_i = 0; pos_i < channel->GetPosValues()->keysCount; pos_i++) {
        //Writing pos vector
        serializer->Serialize(channel->GetPosValues()->values[pos_i]);
        //Write pos key time
        serializer->Serialize(channel->GetPosValues()->times[pos_i]);
    }
    for (uint32 sca_i = 0; sca_i < channel->GetScaleValues()->keysCount; sca_i++) {
        //Writing scale vector
        serializer->Serialize(channel->GetScaleValues()->values[sca_i]);
        //Write pos key time
        serializer->Serialize(channel->GetScaleValues()->times[sca_i]);
    }
    for (uint32 rot_i = 0; rot_i < channel->GetRotationValues()->keysCount; rot_i++) {
        //Writing pos vector
        serializer->Serialize(channel->GetRotationValues()->values[rot_i]);
        //Write pos key time
        serializer->Serialize(channel->GetRotationValues()->times[rot_i]);
    }
}
void AnimationFileExport::write(const std::string& output_file) {
    ByteSerialize* serializer = new ByteSerialize;
    serializer->WriteBytes("zs3manim", 9);
    serializer->Serialize(_AnimationToExport->GetName());
    //Write main animation data
    serializer->Serialize(_AnimationToExport->GetTPS());
    serializer->Serialize(_AnimationToExport->GetDuration());
    serializer->Serialize(_AnimationToExport->GetNumChannels());

    for (uint32 cha_i = 0; cha_i < _AnimationToExport->GetNumChannels(); cha_i++) {
        writeChannel(serializer, cha_i);
    }

    std::ofstream stream;
    stream.open(output_file.c_str(), std::ofstream::binary);
    stream.write((const char*)serializer->GetBytes(), serializer->GetSerializedSize());
    stream.close();

    delete serializer;
}