#ifndef STEP_DRIVER_HPP
#define STEP_DRIVER_HPP

#include "a4988.hpp"
#include "pid.hpp"

namespace StepDriver {

    struct StepDriver {
    public:
        void operator()(float const input_speed_dps, float const sampling_time_s) noexcept;

        float degrees_per_step{};

        A4988::A4988 a4988{};

        Utility::PID<float> pid{};

    private:
    };

}; // namespace StepDriver

#endif // STEP_DRIVER_HPP