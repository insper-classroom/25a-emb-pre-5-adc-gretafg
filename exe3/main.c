#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;
    int len = 0;
    int data_list[5] = {0, 0, 0, 0, 0};

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {

            if (len < 5) {
                data_list[len] = data;
                len++;
            } else {
                data_list[0] = data_list[1];
                data_list[1] = data_list[2];
                data_list[2] = data_list[3];
                data_list[3] = data_list[4];
                data_list[4] = data;
            }

            int soma = 0;
            for (int i = 0; i < len; i++) {
                soma += data_list[i];
            }

            int media = soma / 5;
            printf("%d\n", media);
            

            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}


int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
