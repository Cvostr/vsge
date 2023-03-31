#include "AnimationChannel.hpp"

using namespace VSGE;

VSGE::AnimationChannel::AnimationChannel()
{}

VSGE::AnimationChannel::~AnimationChannel() {

}

const std::string& VSGE::AnimationChannel::GetBoneName() const
{
    return m_channelBoneName; 
}

void VSGE::AnimationChannel::SetBoneName(const std::string& name)
{ 
    m_channelBoneName = name;
}

void AnimationChannel::SetPositionKeysCount(uint32 posKeys)
{
    pos.Allocate(posKeys);
}

void AnimationChannel::SetScaleKeysCount(uint32 scaleKeys)
{
    scale.Allocate(scaleKeys);
}

void AnimationChannel::SetRotationKeysCount(uint32 rotationKeys)
{
    rot.Allocate(rotationKeys);
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

Vec3 AnimationChannel::getPositionInterpolated(double Time, double tps) {
    uint32 index1 = getPositionIndex(Time);
    uint32 index2 = index1 + 1;

    double delta = Time - pos.times[index1];
    delta /= tps;

    Vec3 v1 = pos.values[index1];
    Vec3 v2 = pos.values[index2];

    return lerp(v1, v2, static_cast<float>(delta));
}

Vec3 AnimationChannel::getScaleInterpolated(double Time, double tps) {
    uint32 index1 = getScaleIndex(Time);
    uint32 index2 = index1 + 1;

    double delta = Time - scale.times[index1];
    delta /= tps;

    Vec3 v1 = scale.values[index1];
    Vec3 v2 = scale.values[index2];

    return lerp(v1, v2, static_cast<float>(delta));
}

Quat AnimationChannel::getRotationInterpolated(double Time, double tps) {
    uint32 index1 = getRotationIndex(Time);
    uint32 index2 = index1 + 1;

    double delta = Time - rot.times[index1];
    delta /= tps;

    Quat q1 = rot.values[index1];
    Quat q2 = rot.values[index2];

    return q1.slerp(q2, static_cast<float>(delta));
}