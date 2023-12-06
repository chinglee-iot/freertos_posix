/**
 * Test case that will show if signals are being sent to
 * pthreads other than those created for FreeRTOS tasks.
 *
 * SIGALRM signals sent to non-FreeRTOS pthreads will result
 * in the pthread being incorrect suspended, while
 * the running FreeRTOS task continues to run in
 * parallel with the newly task switched task.
 *
 * Detect this error condition by wrapping the signal handler with an
 * intermediate handler and checking that the currently running thread
 * receiving the signal is never one of the unmanaged pthreads.
 */

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include <thread>
#include <cstdio>
#include <cstring>
#include "task.h"
#include "unistd.h"


// Used to track pthreads created outside of
// FreeRTOS that we check from the signal
// handler to see if a signal was sent to a
// non-FreeRTOS pthread
pthread_t external_pthreads[10];
size_t external_pthreads_in_use;

bool main_running;

bool running;
typedef void (*test_fn)(void* arg);

bool received_signal_on_external_pthread;

#include <signal.h>

struct sigaction original_signal;

void sigalarm_handler(int signal)
{
    // no non-FreeRTOS pthreads should receive the incorrect
    // signal
    auto this_thread = pthread_self();
    for(auto i = 0; i < external_pthreads_in_use; i++)
    {
        if(external_pthreads[i] == this_thread)
        {
            received_signal_on_external_pthread = true;

            const char *str = "******************** received signal on external pthread *********\n";
            const size_t size = strlen(str);
            write(1, str, size);

// NOTE: printf() is NOT safe in the context of a signal handler as
// we could be inside of another printf when the signal handler started
// executing, so we use write(), which is safe inside of signal handlers
//            printf("******* received signal %d on external pthread 0x%ld *********\n", signal, this_thread);
        }
    }

    // call into the original signal
    original_signal.sa_handler(signal);
}

// register sigalarm to detect
// signals incorrectly going to pthreads
void setup_signal()
{
    struct sigaction sigtick;
    sigtick.sa_flags = 0;
    sigtick.sa_handler = sigalarm_handler;
    sigfillset( &sigtick.sa_mask );
    int ret = sigaction( SIGALRM, &sigtick, &original_signal );
    if( ret != 0)
    {
        printf("sigaction failed\n");
    }
}

void a_task_function(void* arg)
{
    printf("%s\n", __FUNCTION__);

    // override the signal handler with our wrapper once
    // the FreeRTOS scheduler has started running, otherwise
    // our handler would be overwritten during the POSIX
    // port startup process
    setup_signal();

    // run test
    vTaskDelay(pdMS_TO_TICKS(2000));

    printf("%s asking main to shut down\n", __FUNCTION__);
    main_running = false;

    vTaskDelay(pdMS_TO_TICKS(250));

    printf("%s ending scheduler\n", __FUNCTION__);
    running = false;
    vTaskEndScheduler();

    // we never expect to get here
    printf("ERROR %s after end scheduler\n", __FUNCTION__);
}

#define TASK_DEPTH 20000
static StackType_t a_stack [TASK_DEPTH];
static StaticTask_t a_task;

static void app_main(void* arg)
{
    printf("%s starting system up\n", __FUNCTION__);

    // create various tasks and objects for the system

    while(main_running)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // only for system testing case
    // clean up tasks / objects
    printf("%s cleaning up and returning\n", __FUNCTION__);

    // it isn't valid to return from a task, FreeRTOS
    // requires that tasks delete themselves
    vTaskDelete(NULL);
}

static StackType_t app_stack [TASK_DEPTH];
static StaticTask_t app_task;


bool run_another_thread = true;

void another_thread()
{
    while(run_another_thread)
    {
        printf("running\n");
        for(volatile uint32_t x = 0; x < 10000; x++) {}
        usleep(100000);
    }
}

int main()
{
    // save the thread
    external_pthreads[external_pthreads_in_use] = pthread_self();
    external_pthreads_in_use++;

    auto t1 = new std::thread(another_thread);
    external_pthreads[external_pthreads_in_use] = t1->native_handle();
    external_pthreads_in_use++;

    // start a task
    xTaskCreateStatic(a_task_function, "a_task", TASK_DEPTH, NULL,
                      1, a_stack, &a_task);

    main_running = true;

    // start the system up
    xTaskCreateStatic(app_main, "app_main", TASK_DEPTH, NULL,
                     1, app_stack, &app_task);

    printf("%s calling vTaskStartScheduler\n", __FUNCTION__);

    vTaskStartScheduler();

    // should return here after ending the scheduler
    printf("After vTaskStartScheduler()\n");

    run_another_thread = false;
    t1->join();
    delete t1;

    return (received_signal_on_external_pthread == false) ? 0 : 1;
}
