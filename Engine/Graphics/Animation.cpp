#include "Animation.hpp"
#include <Math/MathBase.hpp>

using namespace VSGE;

VSGE::Animation::Animation() :
    _numChannels(0),
    m_tps(0),
    channels(nullptr),
    m_duration(0) {}

VSGE::Animation::~Animation() {
    Release();
}

double Animation::GetDuration() const
{ 
    return m_duration;
}

void Animation::SetDuration(double duration) 
{ 
    m_duration = duration;
}

double Animation::GetTPS() const 
{ 
    return m_tps;
}

void Animation::SetTPS(double tps) 
{ 
    m_tps = tps;
}

const std::string& Animation::GetName() 
{ 
    return m_name;
}

void Animation::SetName(const std::string& name) 
{
    m_name = name;
}

uint32 Animation::GetNumChannels() { return _numChannels; }

void Animation::SetNumChannels(uint32 num_channels) {
    _numChannels = num_channels;
    SAFE_RELEASE_ARR(channels)
    //allocate channels
    channels = new AnimationChannel[_numChannels];
}

AnimationChannel* Animation::getChannelByNodeName(const std::string& node_name) {
    for (uint32 i = 0; i < this->_numChannels; i++) {
        AnimationChannel* cha = &this->channels[i];
        if (cha->GetBoneName().compare(node_name) == false) return cha;
    }
    return nullptr;
}

AnimationChannel* Animation::GetChannelByEntityName(const std::string& node_name) {
    uint32 max_size = 0;
    AnimationChannel* result = nullptr;
    for (uint32 i = 0; i < this->_numChannels; i++) {
        AnimationChannel* cha = &this->channels[i];
        if (node_name.rfind(cha->GetBoneName()) == 0 && max_size < cha->GetBoneName().size()) {
            max_size = (uint32)cha->GetBoneName().size();
            result = cha;
        }
    }
    return result;
}

void Animation::CopyTo(Animation* anim) {
    anim->SetName(m_name);
    anim->SetDuration(GetDuration());
    anim->SetTPS(GetTPS());
    anim->SetNumChannels(_numChannels);

    for (uint32 channel_i = 0; channel_i < _numChannels; channel_i++) {
        channels[channel_i].CopyTo(&anim->GetChannels()[channel_i]);
    }
}

void Animation::Release() {
    SAFE_RELEASE_ARR(channels);
}