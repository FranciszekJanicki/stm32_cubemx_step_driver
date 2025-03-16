#include "step_driver.hpp"

namespace StepDriver {

    void StepDriver::operator()(float const input_speed_dps, float const sampling_time_s) noexcept
    {
        // step_period_s = 1 / step_frequency_hz
        // degrees_per_second = degrees_per_step / step_period_s
        // step_frequency_hz = 1 / step_period_s = 1 / (degrees_per_step/degrees_per_second)
        this->a4988.set_frequency(input_speed_dps / this->degrees_per_step);
    }

}; // namespace StepDriver