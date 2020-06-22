#ifndef TENSOR_THREAD
#define TENSOR_THREAD

#ifndef TENSOR_THREAD_STACK_SIZE
#define TENSOR_THREAD_STACK_SIZE (64*1024)
#endif

void tensor_thread_init(void);

void tensor_thread_start(void);

#endif
