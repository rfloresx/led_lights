#include "utils.h"
#include <Arduino.h>

void hex_dump(const void *data, size_t size) {
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        Serial.printf("%02X ", ((unsigned char *)data)[i]);
        if (((unsigned char *)data)[i] >= ' ' &&
            ((unsigned char *)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char *)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size) {
            Serial.printf(" ");
            if ((i + 1) % 16 == 0) {
                Serial.printf("|  %s \n", ascii);
            } else if (i + 1 == size) {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8) {
                    Serial.printf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j) {
                    Serial.printf("   ");
                }
                Serial.printf("|  %s \n", ascii);
            }
        }
    }
}

/******************************************************************************
 * ITaskManager
 ******************************************************************************/
ITaskManager::ITaskManager(uint32_t refresh_rate, BaseType_t core)
    : m_refresh_rate(refresh_rate), m_core(core), m_task_handler(nullptr),
      m_stop(false) {}

void ITaskManager::start(void) {
    this->m_stop = false;
    xTaskCreatePinnedToCore(ITaskManager::ITaskManagerTask, "ITaskManager",
                            10000, this, tskIDLE_PRIORITY + 1,
                            &(this->m_task_handler), this->m_core);
}

void ITaskManager::stop(void) { this->m_stop = true; }

void ITaskManager::ITaskManagerTask(void *ctx) {
    ITaskManager *manager = (ITaskManager *)ctx;
    manager->setup();

    uint32_t sleep_time_ticks =
        pdMS_TO_TICKS((uint32_t)(1000 / manager->m_refresh_rate));

    while (manager->m_stop == false) {
        uint32_t start = xTaskGetTickCount();
        manager->update();
        uint32_t end = xTaskGetTickCount();
        uint32_t delta = (end - start);
        if (delta < sleep_time_ticks) {
            uint32_t sleep_ticks = sleep_time_ticks - delta;
            vTaskDelay(sleep_ticks);
        }
    }

    manager->cleanup();
}
