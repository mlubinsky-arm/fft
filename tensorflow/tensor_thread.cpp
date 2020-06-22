#include "tensor_thread.h"

#include "main_functions.h"

#include "mbed.h"

static Thread* tensor_thread = NULL;
static Semaphore tensor_semaphore(1);
static void tensor_thread_main(void);

void tensor_thread_init(void)
{
    tensor_thread = new Thread(osPriorityNormal, TENSOR_THREAD_STACK_SIZE , NULL, "mqtt_service");
    tensor_semaphore.acquire();
    if (tensor_thread) {
        tensor_thread->start(tensor_thread_main);
    }
}

void tensor_thread_start(void)
{
    tensor_semaphore.release();
}

static void tensor_thread_main(void)
{
    tensor_semaphore.acquire();
    setup();

    for (;;) {
        loop();
        ThisThread::sleep_for(10);  //sensitive for speech capture window frame interval
        ThisThread::yield();
    }
}
