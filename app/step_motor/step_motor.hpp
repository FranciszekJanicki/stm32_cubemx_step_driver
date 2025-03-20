#ifndef STEP_MOTOR_HPP
#define STEP_MOTOR_HPP

#include "a4988.hpp"
#include "pid.hpp"

namespace StepMotor {

    using A4988 = A4988::A4988;
    using Direction = ::A4988::Direction;
    using Microstep = ::A4988::Microstep;

    Direction position_deg_to_direction(float const position_deg, float const last_position_deg) noexcept;

    Direction speed_deg_per_s_to_direction(float const speed_deg_per_s) noexcept;

    std::uint16_t speed_deg_per_s_to_frequency_hz(float const speed_deg_per_s, float const deg_per_step) noexcept;

    float get_processed_position(float const position_deg) noexcept;

    float get_processed_speed(float const speed_deg_per_s,
                              float const last_speed_deg_per_s,
                              float const deg_per_step,
                              float const sampling_time_s,
                              float const max_speed_deg_per_s,
                              float const max_accel_deg_per_ss) noexcept;

    struct StepMotor {
        void set_position(float const position_deg, float const deg_per_step, float const sampling_time_s) noexcept;

        void set_speed(float const speed_deg_per_s, float const deg_per_step, float const sampling_time_s) noexcept;

        A4988 a4988{};

        float min_speed_deg_per_s{};
        float max_speed_deg_per_s{};
        float max_accel_deg_per_ss{};

        float last_position_deg{};
        float last_speed_deg_per_s{};
    };

}; // namespace StepMotor

#endif // STEP_MOTOR_HPP