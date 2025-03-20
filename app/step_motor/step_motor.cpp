#include "step_motor.hpp"

namespace StepMotor {

    Direction position_deg_to_direction(float const position_deg, float const last_position_deg) noexcept
    {
        return std::fmodf(position_deg - last_position_deg, 360.0F) > 0.0F ? Direction::FORWARD : Direction::BACKWARD;
    }

    Direction speed_deg_per_s_to_direction(float const speed_deg_per_s) noexcept
    {
        return speed_deg_per_s > 0.0F ? Direction::FORWARD : Direction::BACKWARD;
    }

    std::uint16_t speed_deg_per_s_to_frequency_hz(float const speed_deg_per_s, float const deg_per_step) noexcept
    {
        return static_cast<std::uint16_t>(std::abs(speed_deg_per_s) / deg_per_step);
    }

    float get_processed_position(float const position_deg) noexcept
    {
        return std::fmodf(position_deg, 360.0F);
    }

    float get_processed_speed(float const speed_deg_per_s,
                              float const last_speed_deg_per_s,
                              float const deg_per_step,
                              float const sampling_time_s,
                              float const max_speed_deg_per_s,
                              float const max_accel_deg_per_ss) noexcept
    {
        auto const clamped_speed_deg_per_s = std::clamp(speed_deg_per_s, -max_speed_deg_per_s, max_speed_deg_per_s);

        auto const accel_deg_per_ss =
            Utility::differentiate(clamped_speed_deg_per_s, last_speed_deg_per_s, sampling_time_s);

        auto const clamped_accel_deg_per_ss = std::clamp(accel_deg_per_ss, -max_accel_deg_per_ss, max_accel_deg_per_ss);

        return clamped_accel_deg_per_ss * sampling_time_s + last_speed_deg_per_s;
    }

    void
    StepMotor::set_position(float const position_deg, float const deg_per_step, float const sampling_time_s) noexcept
    {
        auto const processed_position = get_processed_position(position_deg);

        this->set_speed(Utility::differentiate(processed_position, this->last_position_deg, sampling_time_s),
                        deg_per_step,
                        sampling_time_s);

        this->last_position_deg = processed_position;
    }

    void
    StepMotor::set_speed(float const speed_deg_per_s, float const deg_per_step, float const sampling_time_s) noexcept
    {
        auto const processed_speed = get_processed_speed(speed_deg_per_s,
                                                         this->last_speed_deg_per_s,
                                                         deg_per_step,
                                                         sampling_time_s,
                                                         this->max_speed_deg_per_s,
                                                         this->max_accel_deg_per_ss);
        this->last_speed_deg_per_s = processed_speed;

        auto const direction = speed_deg_per_s_to_direction(processed_speed);
        auto const frequency = speed_deg_per_s_to_frequency_hz(processed_speed, deg_per_step);

        this->a4988.set_direction(direction);
        this->a4988.set_frequency(frequency);
    }

}; // namespace StepMotor