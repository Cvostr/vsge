#pragma once

#include <string>
#include <Math/Vec3.hpp>
#include <Math/Quaternion.hpp>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {

    class AnimationChannel;

    class Animation {
    private:
        double mDuration;
        double mTPS;
        std::string mName;
        //Amount of animation channels
        uint32 mNumChannels;
        //Array of animation channels
        AnimationChannel* channels;
    public:
        
        double GetDuration() { return mDuration; }
        void SetDuration(double duration) { mDuration = duration; }

        double GetTPS() { return mTPS; }
        void SetTPS(double tps) { mTPS = tps; }

        const std::string& GetName() { return mName; }
        void SetName(const std::string& name) { mName = name; }
        
        uint32 GetNumChannels() { return mNumChannels; }
        void SetNumChannels(uint32 num_channels);

        AnimationChannel* GetChannels() {
            return channels;
        }

        //Get pointer to channel by specified node name
        AnimationChannel* getChannelByNodeName(std::string node_name);

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

        TimedValues() {
            values = nullptr;
            times = nullptr;
            keysCount = 0;
        }

        ~TimedValues() {
            if(values)
                delete[] values;
            if(times)
                delete[] times;
        }

    };

    class AnimationChannel {
    private:
        std::string mChannelBoneName;

        TimedValues<Vec3> pos;
        TimedValues<Vec3> scale;
        TimedValues<Quat> rot;

    public:
        
        Animation* anim_ptr;
        
        const std::string& GetBoneName() { return mChannelBoneName; }
        void SetBoneName(const std::string& name) { mChannelBoneName = name; }

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

        Vec3 getPostitionInterpolated(double Time);
        Vec3 getScaleInterpolated(double Time);
        Quat getRotationInterpolated(double Time);

        AnimationChannel();
        ~AnimationChannel();
    };

}