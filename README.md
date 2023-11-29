## Overview

Test repository for the FreeRTOS posix port (the ability to use the FreeRTOS API on a posix system such as Linux)

## To run
```
git submodule update --init --recursive
mkdir build && cd build
cmake ..
make
```

Now you can run any one of the sample applications


## How to use POSIX simulator?

Assumptions
* You may only call FreeRTOS functions from within FreeRTOS tasks.
* It is permissable to use pthread and other C/C++ methods to
  communicate between pthread and FreeRTOS tasks.

Requirements
* vTaskEndScheduler() should be called from within a FreeRTOS Task
* vTaskStartScheduler() returns when vTaskEndScheduler() is called

Your program should:
* Create freertos tasks via xTaskCreate / xTaskCreateStatic
  NOTE: To support shutting down the scheduler one of these
        tasks must be the one that calls vTaskEndScheduler.
* Call vTaskStartScheduler() from any pthread in order to start FreeRTOS.

```
static void a_task_function(void* arg)
{
   // do some stuff
   while(true) { vTaskDelay(pdMS_TO_TICKS(100); }
}

static void app_main(void* arg)
{
    printf("%s starting system up\n", __FUNCTION__);

    ///////
    // create various tasks and objects for the system

    xTaskCreate(a_task_function, "a_task", TASK_DEPTH, NULL,
                    1, &a_task);

    const int loops = 1000;

    while(loop_count < loops)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        loops++;
    }

    // clean up tasks / objects

    // stop the scheduler, this will not return
    vTaskEndScheduler();
}

int main()
{
    // start the system up
    xTaskCreate(app_main, "app_main", TASK_DEPTH, NULL,
                     1, &app_task);

    // blocks until vTaskEndScheduler() is called
    vTaskStartScheduler();

    return 0;
}
```
