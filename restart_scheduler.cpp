/**
 * @brief Testing restarting the scheduler after it has been stoppped.
 *
 */

#include "FreeRTOS/FreeRTOS/Source/portable/ThirdParty/GCC/Posix/portmacro.h"
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

    // should never get here
    printf("ERROR: %s after vTaskDelete\n", __FUNCTION__);
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

    // start the test case
    TaskHandle_t xHandle = xTaskCreateStatic(a_task_function, "test_task", TASK_DEPTH, NULL,
                    1, a_stack, &a_task);
    if(!xHandle)
    {
        printf("task creation failed\n");
        return;
    }

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
    printf("sizeof(StackType_t) %ld\n", sizeof(StackType_t));
    for(int x = 0; x < 5; x++)
    {
        printf("loop %d\n", x);

        printf("app_stack 0x%p, 0x%lx (%ld) bytes\n",
                app_stack,
                sizeof(app_stack),
                sizeof(app_stack));

        // start the system up
        TaskHandle_t xHandle = xTaskCreateStatic(app_main,
                                                 "app_main",
                                                 TASK_DEPTH,
                                                 NULL,
                                                 1,
                                                 app_stack,
                                                 &app_task);
        if(!xHandle)
        {
            printf("task creation failed\n");
            return -1;
        }

        printf("%s calling vTaskStartScheduler\n", __FUNCTION__);

        // blocks until vTaskEndScheduler() is called
        vTaskStartScheduler();

        printf("%s scheduler exited\n", __FUNCTION__);
    }

    return 0;
}
