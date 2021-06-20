#pragma once

#include <string>
#include <Math/Vec3.hpp>
#include <Math/Quaternion.hpp>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {

    class AnimationChannel;

    class Animation {
    private:
        double _duration;
        double _tps;
        std::string _name;
        //Amount of animation channels
        uint32 _numChannels;
        //Array of animation channels
        AnimationChannel* channels;
    public:
        
        double GetDuration();
        void SetDuration(double duration);

        double GetTPS();
        void SetTPS(double tps);

        const std::string& GetName();
        void SetName(const std::string& name);
        
        uint32 GetNumChannels();
        void SetNumChannels(uint32 num_channels);

        AnimationChannel* GetChannels() {
            return channels;
        }

        void SetChannels(AnimationChannel* channels) {
            this->channels = channels;
        }

        //Get pointer to channel by specified node name
        AnimationChannel* getChannelByNodeName(const std::string& node_name);
        AnimationChannel* GetChannelByEntityName(const std::string& node_name);

        void CopyTo(Animation* anim);
        void Release();

        Animation();
        ~Animation();

    };

    template<typename T>
    class TimedValues {
    public:
        T* values;
        double* times;
        uint32 keysCount;

        void Allocate(uint32 count) {
            keysCount = count;
            values = new T[count];
            times = new double[count];
        }

        TimedValues() :
            values(nullptr),
            times(nullptr),
            keysCount(0)
        {}

        TimedValues(T* values, double* times, uint32 count) :
            values(values),
            times(times),
            keysCount(count)
        {}

        ~TimedValues() {
            SAFE_RELEASE_ARR(values);
            SAFE_RELEASE_ARR(times);
        }

    };

    class AnimationChannel {
    private:
        std::string _channelBoneName;

        TimedValues<Vec3> pos;
        TimedValues<Vec3> scale;
        TimedValues<Quat> rot;

    public:
        
        Animation* anim_ptr;
        
        const std::string& GetBoneName() { return _channelBoneName; }
        void SetBoneName(const std::string& name) { _channelBoneName = name; }

        void SetPositionKeysCount(uint32 posKeys) {
            pos.Allocate(posKeys);
        }
        void SetScaleKeysCount(uint32 scaleKeys) {
            scale.Allocate(scaleKeys);
        }
        void SetRotationKeysCount(uint32 rotationKeys) {
            rot.Allocate(rotationKeys);
        }

        TimedValues <Vec3>* GetPosValues() {
            return &pos;
        }
        TimedValues <Vec3>* GetScaleValues() {
            return &scale;
        }
        TimedValues <Quat>* GetRotationValues() {
            return &rot;
        }

        uint32 getPositionIndex(double Time);
        uint32 getScaleIndex(double Time);
        uint32 getRotationIndex(double Time);

        Vec3 getPositionInterpolated(double Time);
        Vec3 getScaleInterpolated(double Time);
        Quat getRotationInterpolated(double Time);

        void CopyTo(AnimationChannel* channel);

        AnimationChannel();
        ~AnimationChannel();
    };

}