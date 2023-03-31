#pragma once

#include "AnimationChannel.hpp"

namespace VSGE {

    class Animation {
    private:
        double m_duration;
        double m_tps;
        std::string m_name;
        //Amount of animation channels
        uint32 _numChannels;
        //Array of animation channels
        AnimationChannel* channels;
    public:
        
        double GetDuration() const;
        void SetDuration(double duration);

        double GetTPS() const;
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

}