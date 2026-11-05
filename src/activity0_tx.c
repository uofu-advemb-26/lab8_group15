#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include <queue.h>
#include <string.h>
#include <stdatomic.h>

#define MAIN_TASK_PRIORITY      ( tskIDLE_PRIORITY + 2UL )
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define PRINT_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define PRINT_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

static struct can2040 cbus;
struct can2040_msg msg;
#define CAN_ID 0x0F5
#define CAN_RX_SIZE 16

typedef struct
{
    uint8_t str[CAN_RX_SIZE]; // Support up to 15 characters plus null-terminator
} can_msg;
QueueHandle_t callback_msg_queue;
volatile atomic_bool tx_error = 0;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    can_msg str;
    // The message was successfully transmitted
    if ((notify == CAN2040_NOTIFY_TX) && (msg.id == CAN_ID))
    {
        // There's an error if the transmitted message says the message was larger than 
        // supported here
        if (msg.dlc > (CAN_RX_SIZE - 1)) // Leave room for null terminator
        {
            atomic_store(&tx_error, true);
            return;
        }

        // Copy the transmitted message with a null terminator
        memcpy(&str.str[0], msg.data, msg.dlc);
        str.str[msg.dlc] = '\0';

        // Send the message to the print function (no delay possible)
        if (xQueueSendFromISR(callback_msg_queue, &str, NULL) != pdPASS)
        {
            atomic_store(&tx_error, true);
        }
    }
    // TX error
    else if (notify == CAN2040_NOTIFY_ERROR)
    {
        atomic_store(&tx_error, true);
    }
}

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 500000;
    uint32_t gpio_rx = 4, gpio_tx = 5;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
    irq_set_priority(PIO0_IRQ_0, PICO_DEFAULT_IRQ_PRIORITY - 1);
    irq_set_enabled(PIO0_IRQ_0, 1);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

void main_task(__unused void *params) {
    // Use an 11-bit ID rather than a 29-bit ID
    msg.id &= ~CAN2040_ID_EFF;
    // Assign a distinctive binary pattern for the ID (0000_1111_0101)
    // and make sure it only occupies 11 bits
    msg.id = CAN_ID & ((1 << 11) - 1);
    // Don't send a remote frame
    msg.id &= ~CAN2040_ID_RTR;

    // Set up the packet to transmit "CANbus"
    char content[] = "CANbus"
    msg.dlc = strlen(content); // Not counting null-terminator
    memcpy(&msg.data[0], content, msg.dlc); // Don't copy null-terminator

    while (1)
    {
        // Send message on CAN bus
        if (can2040_transmit(&cbus, &msg) < 0)
        {
            printf("[TX] Failed to queue message for transmission\n");
        }
        else
        {
            printf("[TX] Queued message for transmission\n");
        }

        // Send message every second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void print_task(__unused void *params)
{
    can_msg received_msg;
    while (1)
    {
        // Block forever until a message has been sent
        xQueueReceive(callback_msg_queue, &received_msg, portMAX_DELAY);

        if (atomic_load(&tx_error))
        {
            printf("[TX] A transmission error occurred\n");
            atomic_store(&tx_error, false);
        }

        // Print the message
        printf("[TX] Message '%s' sent on line\n", received_msg);
    }
}

int main( void )
{
    canbus_setup();
    stdio_init_all();
    
    // Initialize a queue to hold 10 CAN messages from the CAN callback
    callback_msg_queue = xQueueCreate(10, sizeof(can_msg));

    TaskHandle_t task;
    xTaskCreate(main_task, "MainThread",
                MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &task);
    xTaskCreate(print_task, "PrintThread",
                PRINT_TASK_STACK_SIZE, NULL, PRINT_TASK_PRIORITY, NULL);
    vTaskStartScheduler();
    return 0;
}
