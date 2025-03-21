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

    auto constexpr MS1 = GPIO::PA0;
    auto constexpr MS2 = GPIO::PA1;
    auto constexpr MS3 = GPIO::PA2;
    auto constexpr RESET = GPIO::PA3;
    auto constexpr SLEEP = GPIO::PA4;
    auto constexpr DIR = GPIO::PA5;
    auto constexpr ENABLE = GPIO::PA6;

    auto constexpr SAMPLING_TIME = 1.0F;
    auto constexpr STEP_CHANGE = 1.8F;

    using namespace StepDriver;
    using namespace Utility;

    auto pwm_device = PWMDevice{&htim4, TIM_CHANNEL_3, 65535U, 3.3F};

    auto a4988 = A4988::A4988{std::move(pwm_device), MS1, MS2, MS3, RESET, SLEEP, DIR, ENABLE};

    auto pid = PID<float>{};

    auto step_driver = StepDriver::StepDriver{.driver = std::move(a4988), .regulator = pid};

    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);

    while (1) {
        if (tim3_period_elapsed) {
            step_driver.set_speed(300.0F, STEP_CHANGE, SAMPLING_TIME);

            tim3_period_elapsed = false;
            HAL_TIM_Base_Start_IT(&htim3);
        }

        if (tim4_pwm_pulse_finished) {
            step_driver.increase_step_count();

            tim4_pwm_pulse_finished = false;
            HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_3);
        }
    }

    return 0;
}
