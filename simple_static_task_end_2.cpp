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


void a_task_function(void* arg)
{
    printf("%s\n", __FUNCTION__);

    while(true)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // should never reach here
}


#define TASK_DEPTH 20000
static StackType_t a_stack [TASK_DEPTH];
static StaticTask_t a_task;

static StackType_t app_stack [TASK_DEPTH];
static StaticTask_t app_task;

static void app_main(void* arg)
{
    printf("%s starting system up\n", __FUNCTION__);

    ///////
    // create various tasks and objects for the system

    // start the task
    xTaskCreateStatic(a_task_function, "a_task", TASK_DEPTH, NULL,
                    1, a_stack, &a_task);

    int loops_to_run_for = 10;
    int loops = 0;

    while(loops < loops_to_run_for)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        loops++;
    }

    // clean up here
    printf("cleaning up and stopping scheduler\n");

    vTaskEndScheduler();

    printf("after vTaskEndScheduler\n");
}

int main()
{
    // start the system up
    xTaskCreateStatic(app_main, "app_main", TASK_DEPTH, NULL,
                     1, app_stack, &app_task);

    printf("%s calling vTaskStartScheduler\n", __FUNCTION__);

    // blocks until vTaskEndScheduler() is called
    vTaskStartScheduler();

    printf("%s exiting\n", __FUNCTION__);

    return 0;
}
