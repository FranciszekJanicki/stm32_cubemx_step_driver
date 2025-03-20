#ifndef STEP_MOTOR_HPP
#define STEP_MOTOR_HPP

#include "a4988.hpp"
#include "pid.hpp"

namespace StepMotor {

    using Direction = A4988::Direction;

    Direction degrees_per_s_to_direction(float const degrees_per_s) noexcept;

    std::uint16_t degrees_per_s_to_frequency_hz(float const degrees_per_s,
                                                std::uint16_t const steps_per_360,
                                                std::uint16_t const min_frequency_hz,
                                                std::uint16_t const max_frequency_hz) noexcept;

    auto constexpr MAX_FREQ_CHANGE_HZ = 500U;

    struct StepMotor {
        void operator()(float const degrees_per_s, float const sampling_time_s) noexcept;

        A4988::A4988 a4988{};

        std::uint16_t steps_per_360{};
        std::uint16_t min_frequency_hz{};
        std::uint16_t max_frequency_hz{};
    };

}; // namespace StepMotor

#endif // STEP_MOTOR_HPP