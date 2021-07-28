#include "Animation.hpp"
#include <Math/MathBase.hpp>

using namespace VSGE;

VSGE::Animation::Animation() :
    _numChannels(0),
    _tps(0),
    channels(nullptr),
    _duration(0) {}

VSGE::Animation::~Animation() {
    Release();
}

double Animation::GetDuration() { return _duration; }
void Animation::SetDuration(double duration) { _duration = duration; }

double Animation::GetTPS() { return _tps; }
void Animation::SetTPS(double tps) { _tps = tps; }

const std::string& Animation::GetName() { return _name; }
void Animation::SetName(const std::string& name) { _name = name; }

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
    anim->SetName(_name);
    anim->SetDuration(GetDuration());
    anim->SetTPS(GetTPS());
    anim->SetNumChannels(_numChannels);

    for (uint32 channel_i = 0; channel_i < _numChannels; channel_i++) {
        anim->GetChannels()[channel_i].anim_ptr = anim;
        channels[channel_i].CopyTo(&anim->GetChannels()[channel_i]);
    }
}

void Animation::Release() {
    SAFE_RELEASE_ARR(channels);
}


//--------------------------------------ANIMATION CHANNEL-----------------------------


VSGE::AnimationChannel::AnimationChannel()
{
    anim_ptr = nullptr;
}

VSGE::AnimationChannel::~AnimationChannel() {

}

void AnimationChannel::CopyTo(AnimationChannel* channel) {
    channel->SetBoneName(GetBoneName());

    channel->SetPositionKeysCount(pos.keysCount);
    channel->SetScaleKeysCount(scale.keysCount);
    channel->SetRotationKeysCount(rot.keysCount);

    for (uint32 pos_i = 0; pos_i < pos.keysCount; pos_i++) {
        //copy pos vector
        channel->GetPosValues()->values[pos_i] = pos.values[pos_i];
        //copy position timings
        channel->GetPosValues()->times[pos_i] = pos.times[pos_i];
    }
    for (uint32 sca_i = 0; sca_i < scale.keysCount; sca_i++) {
        //reading scale vector
        channel->GetScaleValues()->values[sca_i] = scale.values[sca_i];
        //Read scale timings
        channel->GetScaleValues()->times[sca_i] = scale.times[sca_i];
    }
    for (uint32 rot_i = 0; rot_i < rot.keysCount; rot_i++) {
        //reading scale vector
        channel->GetRotationValues()->values[rot_i] = rot.values[rot_i];
        //Read rotation timings
        channel->GetRotationValues()->times[rot_i] = rot.times[rot_i];
    }
}

uint32 AnimationChannel::getPositionIndex(double Time) {
    for (uint32 i = 0; i < this->pos.keysCount - 1; i++) {
        double _time = pos.times[i + 1];
        if (_time > Time) {
            return i;
        }
    }
    //assert(0);
    return 0;
}
uint32 AnimationChannel::getScaleIndex(double Time) {
    for (uint32 i = 0; i < this->scale.keysCount - 1; i++) {
        double _time = scale.times[i + 1];
        if (_time > Time)
            return i;
    }
    //assert(0);
    return 0;
}
uint32 VSGE::AnimationChannel::getRotationIndex(double Time) {
    for (uint32 i = 0; i < this->rot.keysCount - 1; i++) {
        double _time = rot.times[i + 1];
        if (_time > Time) {
            return i;
        }
    }
    //assert(0);
    return 0;
}

Vec3 AnimationChannel::getPositionInterpolated(double Time) {
    uint32 index1 = getPositionIndex(Time);
    uint32 index2 = index1 + 1;

    double delta = Time - pos.times[index1];
    delta /= anim_ptr->GetTPS();

    Vec3 v1 = pos.values[index1];
    Vec3 v2 = pos.values[index2];

    return lerp(v1, v2, static_cast<float>(delta));
}

Vec3 AnimationChannel::getScaleInterpolated(double Time) {
    uint32 index1 = getScaleIndex(Time);
    uint32 index2 = index1 + 1;

    double delta = Time - scale.times[index1];
    delta /= anim_ptr->GetTPS();

    Vec3 v1 = scale.values[index1];
    Vec3 v2 = scale.values[index2];

    return lerp(v1, v2, static_cast<float>(delta));
}

Quat AnimationChannel::getRotationInterpolated(double Time) {
    uint32 index1 = getRotationIndex(Time);
    uint32 index2 = index1 + 1;

    double delta = Time - rot.times[index1];
    delta /= anim_ptr->GetTPS();

    Quat q1 = rot.values[index1];
    Quat q2 = rot.values[index2];

    return q1.slerp(q2, static_cast<float>(delta));
}