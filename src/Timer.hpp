#ifndef _Timer
#define _Timer

#include <chrono>

class Timer{
public:
    Timer():m_startTime(std::chrono::high_resolution_clock::now()){}

    /** * \brief Sets start time to now */
    void start(){
        m_startTime = std::chrono::high_resolution_clock::now();
    }

    float Timer::getElapsed() const{
        std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - m_startTime;
        return duration.count();
    }

private:
    std::chrono::high_resolution_clock::time_point m_startTime;
};

#endif
