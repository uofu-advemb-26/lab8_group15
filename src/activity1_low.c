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
#define MAIN_TASK_STACK_SIZE 512
#define PRINT_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define PRINT_TASK_STACK_SIZE 512

static struct can2040 cbus;
struct can2040_msg msg;
#define CAN_ID 0x7FF
#define CAN_RX_SIZE 9

typedef struct
{
    uint8_t str[CAN_RX_SIZE]; // Support up to 8 characters plus null-terminator
    uint8_t direction;
} can_msg;
QueueHandle_t callback_msg_queue;
volatile atomic_bool can_error = 0;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    can_msg str;
    // The message was successfully transmitted
    if ((notify & CAN2040_NOTIFY_TX) && (msg->id == CAN_ID))
    {
        // There's an error if the transmitted message says the message was larger than 
        // supported here (only transmission sizes of 8 bytes are supported)
        if (msg->dlc > (CAN_RX_SIZE - 1)) // Leave room for null terminator
        {
            atomic_store(&can_error, true);
            return;
        }

        // Copy the transmitted message with a null terminator
        memcpy(&str.str[0], msg->data, msg->dlc);
        str.str[msg->dlc] = '\0';
        str.direction = 1;

        // Send the message to the print function (no delay possible)
        if (xQueueSendFromISR(callback_msg_queue, &str, NULL) != pdPASS)
        {
            atomic_store(&can_error, true);
        }
    }
    // A message was received
    else if (notify & CAN2040_NOTIFY_RX)
    {
        // There's an error if the transmitted message says the message was larger than 
        // supported here (only transmission sizes of 8 bytes are supported)
        if (msg->dlc > (CAN_RX_SIZE - 1)) // Leave room for null terminator
        {
            atomic_store(&can_error, true);
            return;
        }

        // Copy the transmitted message with a null terminator
        memcpy(&str.str[0], msg->data, msg->dlc);
        str.str[msg->dlc] = '\0';
        str.direction = 2;

        // Send the message to the print function (no delay possible)
        if (xQueueSendFromISR(callback_msg_queue, &str, NULL) != pdPASS)
        {
            atomic_store(&can_error, true);
        }
    }
    // TX error
    else if (notify & CAN2040_NOTIFY_ERROR)
    {
        atomic_store(&can_error, true);
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
    // Program the PIO, setup callbacks, and start processing CAN messages
    canbus_setup();

    // Use an 11-bit ID rather than a 29-bit ID
    msg.id &= ~CAN2040_ID_EFF;
    // Assign a distinctive binary pattern for the ID (0000_1111_0101)
    // and make sure it only occupies 11 bits
    msg.id = CAN_ID & ((1 << 11) - 1);
    // Don't send a remote frame
    msg.id &= ~CAN2040_ID_RTR;

    // Set up the packet to transmit "CANbus"
    char content[] = "LowPRI";
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

        // Print messages when sent
        if (xQueueReceive(callback_msg_queue, &received_msg, pdMS_TO_TICKS(1000)) == pdTRUE)
        {
            if (received_msg.direction == 1)
            {
                printf("[TX] Message '%s' sent on line\n", received_msg.str);
            }
            else if (received_msg.direction == 2)
            {
                printf("[RX] Received message '%s'\n", received_msg.str);
            }
        }

        // Output if an error has occurred
        if (atomic_load(&can_error))
        {
            printf("[CAN] An error occurred\n");
            atomic_store(&can_error, false);
        }
    }
}

int main( void )
{
    stdio_init_all();

    // Give time for terminal to be attached
    sleep_ms(5000);
    
    // Initialize a queue to hold 10 CAN messages from the CAN callback
    if ((callback_msg_queue = xQueueCreate(10, sizeof(can_msg))) != NULL)
    {
        printf("[CAN] Initialized queue\n");
    }

    TaskHandle_t task;
    xTaskCreate(main_task, "MainThread",
                MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &task);
    xTaskCreate(print_task, "PrintThread",
                PRINT_TASK_STACK_SIZE, NULL, PRINT_TASK_PRIORITY, NULL);
    vTaskStartScheduler();
    return 0;
}
