#include "main.h"
#include "a4988.hpp"
#include "gpio.h"
#include "pid.hpp"
#include "step_motor.hpp"
#include "tim.h"
#include "usart.h"
#include <cstdio>
#include <stm32l476xx.h>

namespace {

    volatile auto interrupt = false;

}; // namespace

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM3) {
        interrupt = true;
    }
}

int main()
{
    HAL_Init();
    SystemClock_Config();

    MX_USART2_UART_Init();
    MX_TIM1_Init();
    MX_TIM3_Init();
    MX_GPIO_Init();

    using namespace A4988;
    using namespace StepMotor;
    using namespace Utility;

    auto constexpr MS1 = GPIO::PA0;
    auto constexpr MS2 = GPIO::PA1;
    auto constexpr MS3 = GPIO::PA2;
    auto constexpr RESET = GPIO::PA3;
    auto constexpr SLEEP = GPIO::PA4;
    auto constexpr DIR = GPIO::PA5;
    auto constexpr ENABLE = GPIO::PA6;

    auto constexpr SAMPLING_TIME_S = 5;

    auto pwm_device = PWMDevice{&htim1, TIM_CHANNEL_4, 65535U, 3.3F};

    auto a4988 = A4988::A4988{std::move(pwm_device), MS1, MS2, MS3, RESET, SLEEP, DIR, ENABLE};

    auto step_driver = StepMotor::StepMotor{.a4988 = std::move(a4988),
                                            .steps_per_360 = 200U,
                                            .min_frequency_hz = 100U,
                                            .max_frequency_hz = 6000U};

    HAL_TIM_Base_Start_IT(&htim3);

    auto degrees_per_s = 0U;

    while (true) {
        if (interrupt) {
            step_driver(degrees_per_s += 60, SAMPLING_TIME_S);

            interrupt = false;
            HAL_TIM_Base_Start_IT(&htim3);
        }
    }

    return 0;
}
