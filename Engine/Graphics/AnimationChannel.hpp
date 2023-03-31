#pragma once

#include <Core/VarTypes/Base.hpp>
#include <string>
#include <Math/Vec3.hpp>
#include <Math/Quaternion.hpp>

namespace VSGE {

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
        std::string m_channelBoneName;

        TimedValues<Vec3> pos;
        TimedValues<Vec3> scale;
        TimedValues<Quat> rot;

    public:

        const std::string& GetBoneName() const;
        void SetBoneName(const std::string& name);

        void SetPositionKeysCount(uint32 posKeys);
        void SetScaleKeysCount(uint32 scaleKeys);
        void SetRotationKeysCount(uint32 rotationKeys);

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

        Vec3 getPositionInterpolated(double Time, double tps);
        Vec3 getScaleInterpolated(double Time, double tps);
        Quat getRotationInterpolated(double Time, double tps);

        void CopyTo(AnimationChannel* channel);

        AnimationChannel();
        ~AnimationChannel();
    };
}