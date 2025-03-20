#include "step_motor.hpp"

namespace StepMotor {

    Direction degrees_per_s_to_direction(float const degrees_per_s) noexcept
    {
        return degrees_per_s ? Direction::FORWARD : Direction::BACKWARD;
    }

    std::uint16_t degrees_per_s_to_frequency_hz(float const degrees_per_s,
                                                std::uint16_t const steps_per_360,
                                                std::uint16_t const min_frequency_hz,
                                                std::uint16_t const max_frequency_hz) noexcept
    {
        auto const frequency = std::abs(degrees_per_s) * static_cast<float>(steps_per_360) / 360.0F;
        return std::clamp(static_cast<std::uint16_t>(frequency), min_frequency_hz, max_frequency_hz);
    }

    void StepMotor::operator()(float const degrees_per_s, float const sampling_time) noexcept
    {
        auto const direction = degrees_per_s_to_direction(degrees_per_s);
        auto const frequency = degrees_per_s_to_frequency_hz(degrees_per_s,
                                                             this->steps_per_360,
                                                             this->min_frequency_hz,
                                                             this->max_frequency_hz);

        this->a4988.set_direction(direction);
        this->a4988.set_frequency(frequency);
    }

}; // namespace StepMotor