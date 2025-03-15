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

    volatile auto interrupt = false;
};

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
    using namespace StepDriver;
    using namespace Utility;

    auto constexpr MS1 = GPIO::PA0;
    auto constexpr MS2 = GPIO::PA1;
    auto constexpr MS3 = GPIO::PA2;
    auto constexpr RESET = GPIO::PA3;
    auto constexpr SLEEP = GPIO::PA4;
    auto constexpr DIR = GPIO::PA5;
    auto constexpr ENABLE = GPIO::PA6;

    auto constexpr INPUT_SPEED_DPS = 10.0F;
    auto constexpr SAMPLING_TIME_S = 0.1F;
    auto constexpr DEGREES_PER_STEP = 1.8F;

    auto pwm_device = PWMDevice{&htim1, TIM_CHANNEL_4, 65535U, 3.3F};

    auto a4988 = A4988::A4988{std::move(pwm_device), MS1, MS2, MS3, RESET, SLEEP, DIR, ENABLE};

    auto pid = PID<float>{.proportion_gain = 1.0F, .integral_gain = 0.0F, .derivative_gain = 0.0F};

    auto step_driver =
        StepDriver::StepDriver{.degrees_per_step = DEGREES_PER_STEP, .a4988 = std::move(a4988), .pid = pid};

    HAL_TIM_Base_Start_IT(&htim3);

    while (true) {
        if (interrupt) {
            step_driver(INPUT_SPEED_DPS, SAMPLING_TIME_S);

            HAL_TIM_Base_Start_IT(&htim3);
            interrupt = false;
        }
    }

    return 0;
}
