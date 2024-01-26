/**
 * @brief Test of the ability for a POSIX port freeRTOS program to exit
 * cleanly after ending the scheduler.
 *
 * Program should exit after call to
 * vTaskEndScheduler().
 *
 */

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include <thread>
#include <cstdio>
#include <cstring>
#include "task.h"
#include "unistd.h"


#define TASK_DEPTH 20000

static TaskHandle_t a_task;
static TaskHandle_t app_task;

bool main_running;

void a_task_function(void* arg)
{
    printf("%s\n", __FUNCTION__);

    // run test
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("%s asking main to shut down\n", __FUNCTION__);
    main_running = false;

    vTaskDelay(pdMS_TO_TICKS(250));

    printf("%s ending scheduler\n", __FUNCTION__);
    vTaskEndScheduler();

    // should never reach here
    printf("ERROR %s after end scheduler\n", __FUNCTION__);
}

static void app_main(void* arg)
{
    printf("%s starting system up\n", __FUNCTION__);

    ///////
    // create various tasks and objects for the system

    // start the task
    xTaskCreate(a_task_function,
                "a_task",
                TASK_DEPTH,
                NULL,
                1,
                &a_task);

    while(main_running)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // only for system testing case
    // clean up tasks / objects
    printf("%s calling vTaskDelete\n", __FUNCTION__);

    // it isn't valid to return from a task, FreeRTOS
    // requires that tasks delete themselves
    vTaskDelete(NULL);
}

int main()
{
    main_running = true;

    // start the system up
    xTaskCreate(app_main,
                "app_main",
                TASK_DEPTH,
                NULL,
                1,
                &app_task);

    printf("%s calling vTaskStartScheduler\n", __FUNCTION__);

    // blocks until vTaskEndScheduler() is called
    vTaskStartScheduler();

    // Delete the task calling vTaskEndScheduler.
    vTaskDelete( xTaskGetCurrentTaskHandle() );

    printf("%s exiting\n", __FUNCTION__);

    return 0;
}
