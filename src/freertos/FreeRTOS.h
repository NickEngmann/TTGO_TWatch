#ifndef FREERTOS_H
#define FREERTOS_H

#ifdef NATIVE_BUILD

/* Minimal FreeRTOS stubs - only types needed for compilation */

typedef void* EventGroupHandle_t;
typedef void* SemaphoreHandle_t;
typedef void* TaskHandle_t;
typedef void* QueueHandle_t;
typedef void* TimerHandle_t;
typedef void* StaticTimer_t;
typedef void* StaticTask_t;
typedef void* StaticQueue_t;
typedef void* StaticSemaphore_t;
typedef void* StaticEventGroup_t;
typedef void* StaticMutex_t;
typedef void* StaticBinarySemaphore_t;
typedef void* StaticCountingSemaphore_t;

typedef uint32_t BaseType_t;
typedef uint32_t UBaseType_t;
typedef uint32_t TickType_t;
typedef uint32_t EventBits_t;
typedef void (*TaskFunction_t)(void*);
typedef uint8_t StackType_t;

#define pdTRUE 1
#define pdFALSE 0
#define portYIELD_FROM_ISR(x) do {} while(0)
#define configASSERT(x) do {} while(0)
#define _BV(x) (1 << (x))

#endif /* NATIVE_BUILD */

#endif /* FREERTOS_H */
