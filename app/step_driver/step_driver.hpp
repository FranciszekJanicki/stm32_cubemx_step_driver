#ifndef STEP_DRIVER_HPP
#define STEP_DRIVER_HPP

#include "a4988.hpp"
#include "pid.hpp"

namespace StepDriver {

    using namespace A4988;
    using namespace Utility;

    Direction speed_to_direction(float const speed) noexcept;

    std::uint16_t speed_to_frequency(float const speed, float const step_change) noexcept;

    float get_processed_speed(float const speed,
                              float const prev_speed,
                              float const step_change,
                              float const sampling_time,
                              float const max_speed,
                              float const max_acceleration) noexcept;

    struct StepDriver {
        void increase_step_count() noexcept;

        void set_position(float const position, float const step_change, float const sampling_time) noexcept;
        void set_speed(float const speed, float const step_change, float const sampling_time) noexcept;
        void set_acceleration(float const accel, float const step_change, float const sampling_time) noexcept;

        float get_position(float const step_change, float const sampling_time) noexcept;
        float get_speed(float const step_change, float const sampling_time) noexcept;
        float get_acceleration(float const step_change, float const sampling_time) noexcept;

        A4988::A4988 driver{};
        Utility::PID<float> regulator{};

        std::uint64_t step_count{};

        float max_position{};
        float max_speed{};
        float max_acceleration{};

        float prev_position{};
        float prev_speed{};
        float prev_acceleration{};

        float prev_error_position{};
        float prev_error_speed{};
        float prev_error_acceleration{};
    };

}; // namespace StepDriver

#endif // STEP_DRIVER_HPP