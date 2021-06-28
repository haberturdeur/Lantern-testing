#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "library/BlackBox_I2C.hpp"
#include "library/BlackBox_MPU.hpp"
#include "library/BlackBox_Ring.hpp"
#include "library/BlackBox_LDC.hpp"
#include "library/BlackBox_Touchpad.hpp"
#include "library/BlackBox_Power.hpp"
#include <esp_log.h>
#include <iostream>

using namespace BlackBox;
using namespace std;

extern "C" {
void app_main()
{

    esp_log_level_set("*", ESP_LOG_INFO);

    int counter = 0;

    Power power;
    power.init();
    power.turnOn();
    power.turnOn5V();

    MPU mpu;
    mpu.init();
    mpu.syncCache();
    for (int i = 0; i < MPU6050Regs::MaxAddress; i++)
        cout << hex << i << " : " << static_cast<int>(mpu.regs().regs[i]) << endl;

    Ring ring;
    ring.enableDarkMode();

    LDC ldc;
    ldc.init();
    ldc.configure();

    Touchpad touchpad(16, 0b1110, 1, 1, 1, 1);

    touchpad.init(&ldc);

    cout << "-----------------------------------------------------" << endl;
    cout << "acc_x\t"
         << "acc_y\t"
         << "acc_z\t"
         << "gyro_x\t"
         << "gyro_y\t"
         << "gyro_z\t"
         << "ldc_0\t"
         << "ldc_1\t"
         << "ldc_2\t"
         << "ldc_3\t"
         << "x\t"
         << "y\t"
         << "pressure\t"
         << dec
         << endl;

    while (true) {
        auto data = mpu.scale(true);
        ldc.syncChannels();
        auto coords = touchpad.calculate();
        // mpu.scale(true);
        // auto data = mpu.raw();
        cout << data.accelerometer.x << "\t";
        cout << data.accelerometer.y << "\t";
        cout << data.accelerometer.z << "\t";

        cout << data.gyroscope.x << "\t";
        cout << data.gyroscope.y << "\t";
        cout << data.gyroscope.z << "\t";

        cout << ldc[0] << "\t";
        cout << ldc[1] << "\t";
        cout << ldc[2] << "\t";
        cout << ldc[3] << "\t";

        cout << coords.x << "\t";
        cout << coords.y << "\t";
        cout << coords.pressure << endl;


        ring.clear();
        ring.drawArc(Rgb(255,255,255), ((counter % 120) - (counter % 60)) ? (counter % 60) : 0, ((counter % 120) - (counter % 60)) ? - 1 : (counter % 60), ArcType::Clockwise);
        ring.show();

        counter++;

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
}