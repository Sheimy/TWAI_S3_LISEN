#include <Arduino.h>
#include "driver/twai.h"
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
/* --------------------------- Tasks and Functions -------------------------- */
static void twai_receive_task(void *arg)
{
  while (1)
  {
    twai_message_t message;
    if (twai_receive(&message, pdMS_TO_TICKS(10000)) == ESP_OK)
    {
      //printf("Message received\n");
      // Process received message
     /* if (message.extd)
      {
        printf("Message is in Extended Format\n");
      }
      else
      {
        printf("Message is in Standard Format\n");
      }*/
      printf("0x%03x :", message.identifier);
      if (!(message.rtr))
      {
        for (int i = 0; i < message.data_length_code; i++)
        {
          printf("0x%02x ", message.data[i]);
        }
        printf("\n");
      }
    }
    vTaskDelay(50 / portTICK_RATE_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_11, GPIO_NUM_10, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  // Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
  {
    printf("Driver installed\n");
  }
  else
  {
    printf("Failed to install driver\n");
    return;
  }
  // Start TWAI driver
  if (twai_start() == ESP_OK)
  {
    printf("Driver started\n");
  }
  else
  {
    printf("Failed to start driver\n");
    return;
  }

  xTaskCreate(&twai_receive_task, "hello_task", 4096, NULL, 5, NULL);
}

void loop()
{
}