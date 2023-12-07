#ifndef _Lerp
#define _Lerp

namespace Lerp
{
    inline constexpr const float lerp(float time, float start, float end){
        return (end - start) * time + start;
    }

    inline constexpr const float square(float t){
        return t * t;
    }

    inline constexpr const float raise(float t, int power){
        float raisedValue = t;

        for(int i = 0; i < power; ++i){
            raisedValue *= t;
        }

        return raisedValue;
    }
    
    inline constexpr const float flip(float t){
        return 1 - t;
    }

    inline constexpr const float smoothStop(float t){
        return flip(square(flip(t)));
    }

    inline constexpr const float smoothStopRaised(float t, int power){
        return flip(raise(flip(t),power));
    }

} // namespace Lerp


#endif