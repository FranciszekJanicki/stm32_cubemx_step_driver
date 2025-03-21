#include "step_driver.hpp"
#include <cinttypes>

namespace StepDriver {

    Direction speed_to_direction(float const speed) noexcept
    {
        return speed > 0.0F ? Direction::FORWARD : Direction::BACKWARD;
    }

    std::uint16_t speed_to_frequency(float const speed, float const step_change) noexcept
    {
        return static_cast<std::uint16_t>(std::abs(speed) / step_change);
    }

    float get_processed_speed(float const speed,
                              float const prev_speed,
                              float const step_change,
                              float const sampling_time,
                              float const max_speed,
                              float const max_acceleration) noexcept
    {
        auto const clamped_speed = std::clamp(speed, -max_speed, max_speed);
        auto const acceleration = Utility::differentiate(clamped_speed, prev_speed, sampling_time);
        auto const clamped_acceleration = std::clamp(acceleration, -max_acceleration, max_acceleration);

        return clamped_acceleration * sampling_time + prev_speed;
    }

    void StepDriver::increase_step_count() noexcept
    {
        ++this->step_count;
    }

    void StepDriver::set_position(float const position, float const step_change, float const sampling_time) noexcept
    {
        auto const error_position = position - this->get_position(step_change, sampling_time);
        auto const error_speed = Utility::differentiate(error_position, this->prev_error_position, sampling_time);
        auto const control_speed = this->regulator(error_speed, sampling_time);

        this->driver.set_direction(speed_to_direction(control_speed));
        this->driver.set_frequency(speed_to_frequency(control_speed, step_change));
    }

    void StepDriver::set_speed(float const speed, float const step_change, float const sampling_time) noexcept
    {
        auto const error_speed = speed - this->get_speed(step_change, sampling_time);
        auto const control_speed = this->regulator(error_speed, sampling_time);

        this->driver.set_direction(speed_to_direction(control_speed));
        this->driver.set_frequency(speed_to_frequency(control_speed, step_change));
    }

    void
    StepDriver::set_acceleration(float const acceleration, float const step_change, float const sampling_time) noexcept
    {
        auto const error_acceleration = acceleration - this->get_acceleration(step_change, sampling_time);
        auto const error_speed = Utility::integrate(error_acceleration, this->prev_error_acceleration, sampling_time);
        auto const control_speed = this->regulator(error_speed, sampling_time);

        this->driver.set_direction(speed_to_direction(control_speed));
        this->driver.set_frequency(speed_to_frequency(control_speed, step_change));
    }

    float StepDriver::get_position(float const step_change, float const sampling_time) noexcept
    {
        return std::fmodf(step_change * this->step_count, 360.0F);
    }

    float StepDriver::get_speed(float const step_change, float const sampling_time) noexcept
    {
        auto const position = this->get_position(step_change, sampling_time);

        return Utility::differentiate(position, std::exchange(this->prev_position, position), sampling_time);
    }

    float StepDriver::get_acceleration(float const step_change, float const sampling_time) noexcept
    {
        auto const speed = this->get_speed(step_change, sampling_time);

        return Utility::differentiate(speed, std::exchange(this->prev_speed, speed), sampling_time);
    }

}; // namespace StepDriver