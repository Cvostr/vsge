#include "Animation.hpp"
#include <Math/MathBase.hpp>

using namespace VSGE;

VSGE::Animation::Animation() :
    mNumChannels(0),
    mTPS(0),
    channels(nullptr),
    mDuration(0) {}

VSGE::Animation::~Animation() {
    delete[] this->channels;
}

VSGE::AnimationChannel::AnimationChannel()
{
    anim_ptr = nullptr;
}

VSGE::AnimationChannel::~AnimationChannel() {

}

void VSGE::Animation::SetNumChannels(uint32 num_channels) {
    mNumChannels = num_channels;
    if (channels) {
        delete[] channels;
    }
    //allocate channels
    channels = new AnimationChannel[mNumChannels];
}

VSGE::AnimationChannel* VSGE::Animation::getChannelByNodeName(std::string node_name) {
    for (unsigned int i = 0; i < this->mNumChannels; i++) {
        AnimationChannel* cha = &this->channels[i];
        if (cha->GetBoneName().compare(node_name) == false) return cha;
    }
    return nullptr;
}

unsigned int VSGE::AnimationChannel::getPositionIndex(double Time) {
    for (unsigned int i = 0; i < this->pos.keysCount - 1; i++) {
        double _time = pos.times[i + 1];
        if (_time > Time) {
            return i;
        }
    }
    //assert(0);
    return 0;
}
unsigned int VSGE::AnimationChannel::getScaleIndex(double Time) {
    for (unsigned int i = 0; i < this->scale.keysCount - 1; i++) {
        double _time = scale.times[i + 1];
        if (_time > Time)
            return i;
    }
    //assert(0);
    return 0;
}
unsigned int VSGE::AnimationChannel::getRotationIndex(double Time) {
    for (unsigned int i = 0; i < this->rot.keysCount - 1; i++) {
        double _time = rot.times[i + 1];
        if (_time > Time) {
            return i;
        }
    }
    //assert(0);
    return 0;
}

Vec3 VSGE::AnimationChannel::getPostitionInterpolated(double Time) {
    unsigned int index1 = getPositionIndex(Time);
    unsigned int index2 = index1 + 1;

    double delta = Time - pos.times[index1];
    delta /= anim_ptr->GetTPS();

    Vec3 v1 = pos.values[index1];
    Vec3 v2 = pos.values[index2];

    return lerp(v1, v2, static_cast<float>(delta));
}

Vec3 VSGE::AnimationChannel::getScaleInterpolated(double Time) {
    unsigned int index1 = getScaleIndex(Time);
    unsigned int index2 = index1 + 1;

    double delta = Time - scale.times[index1];
    delta /= anim_ptr->GetTPS();

    Vec3 v1 = scale.values[index1];
    Vec3 v2 = scale.values[index2];

    return lerp(v1, v2, static_cast<float>(delta));
}

Quat VSGE::AnimationChannel::getRotationInterpolated(double Time) {
    unsigned int index1 = getRotationIndex(Time);
    unsigned int index2 = index1 + 1;

    double delta = Time - rot.times[index1];
    delta /= anim_ptr->GetTPS();

    Quat q1 = rot.values[index1];
    Quat q2 = rot.values[index2];

    return q1.slerp(q2, static_cast<float>(delta));
}
