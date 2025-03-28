#include "main.h"
#include "a4988.hpp"
#include "gpio.h"
#include "pid.hpp"
#include "step_driver.hpp"
#include "tim.h"
#include "usart.h"
#include <cstdio>
#include <stm32l476xx.h>

namespace {

    volatile auto tim3_period_elapsed = false;
    volatile auto tim4_pwm_pulse_finished = false;

}; // namespace

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM4) {
        tim4_pwm_pulse_finished = true;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM3) {
        tim3_period_elapsed = true;
    }
}

int main()
{
    HAL_Init();
    SystemClock_Config();

    MX_USART2_UART_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_GPIO_Init();

    using namespace StepDriver;

    auto constexpr MS1 = GPIO::PB4;
    auto constexpr MS2 = GPIO::PB5;
    auto constexpr MS3 = GPIO::PB6;
    auto constexpr RESET = GPIO::PA3;
    auto constexpr SLEEP = GPIO::PA4;
    auto constexpr DIR = GPIO::PB7;
    auto constexpr ENABLE = GPIO::PA6;

    auto constexpr SAMPLING_TIME = 1.0F;

    using namespace StepDriver;
    using namespace Utility;

    auto pwm_device = PWMDevice{&htim4, TIM_CHANNEL_3, 65535U, 3.3F};

    auto a4988 = A4988::A4988{std::move(pwm_device), MS1, MS2, MS3, RESET, SLEEP, DIR, ENABLE};

    auto pid = PID<float>{.proportion_gain = 10.0F,
                          .integral_gain = 0.0F,
                          .derivative_gain = 0.0F,
                          .time_constant = 0.0F,
                          .control_gain = 0.0F,
                          .saturation = 100.0F};

    auto step_driver = StepDriver::StepDriver{.driver = std::move(a4988), .regulator = pid, .steps_per_360 = 200U};

    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);

    auto i = 0.0F;

    while (1) {
        if (tim3_period_elapsed) {
            step_driver.set_speed(i += 3, SAMPLING_TIME);

            tim3_period_elapsed = false;
            HAL_TIM_Base_Start_IT(&htim3);
        }

        if (tim4_pwm_pulse_finished) {
            step_driver.update_step_count();

            tim4_pwm_pulse_finished = false;
            HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
        }
    }
}
