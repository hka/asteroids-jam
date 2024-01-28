#ifndef _Lerp
#define _Lerp

namespace LerpLib
{
    float lerp(float time, float start, float end){
        return (end - start) * time + start;
    }

    float square(float t){
        return t * t;
    }

    float raise(float t, int power){
        float raisedValue = t;

        for(int i = 0; i < power; ++i){
            raisedValue *= t;
        }

        return raisedValue;
    }
    
    float flip(float t){
        return 1 - t;
    }

    float smoothStop(float t){
        return flip(square(flip(t)));
    }

    float smoothStopRaised(float t, int power){
        return flip(raise(flip(t),power));
    }

} // namespace Lerp


#endif