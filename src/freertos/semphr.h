#ifndef FREERTOS_SEMPHR_H
#define FREERTOS_SEMPHR_H

#ifdef NATIVE_BUILD

#include <stdint.h>
#include "FreeRTOS.h"

/* FreeRTOS semaphore stubs */

typedef SemaphoreHandle_t SemaphoreHandle_t;
typedef void* QueueHandle_t;

static inline SemaphoreHandle_t xSemaphoreCreateMutex(void) {
    return (SemaphoreHandle_t)0x12345678;
}

static inline SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount, UBaseType_t uxInitialCount) {
    (void)uxMaxCount;
    (void)uxInitialCount;
    return (SemaphoreHandle_t)0x12345678;
}

static inline BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait) {
    (void)xSemaphore;
    (void)xTicksToWait;
    return pdTRUE;
}

static inline BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore) {
    (void)xSemaphore;
    return pdTRUE;
}

static inline BaseType_t xSemaphoreTakeFromISR(SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xSemaphore;
    (void)pxHigherPriorityTaskWoken;
    return pdTRUE;
}

static inline BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xSemaphore;
    (void)pxHigherPriorityTaskWoken;
    return pdTRUE;
}

static inline void vSemaphoreDelete(SemaphoreHandle_t xSemaphore) {
    (void)xSemaphore;
}

static inline QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize) {
    (void)uxQueueLength;
    (void)uxItemSize;
    return (QueueHandle_t)0x12345678;
}

static inline BaseType_t xQueueSendToBack(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait) {
    (void)xQueue;
    (void)pvItemToQueue;
    (void)xTicksToWait;
    return pdTRUE;
}

static inline BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait) {
    (void)xQueue;
    (void)pvBuffer;
    (void)xTicksToWait;
    return pdTRUE;
}

static inline BaseType_t xQueueSend(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait) {
    (void)xQueue;
    (void)pvItemToQueue;
    (void)xTicksToWait;
    return pdTRUE;
}

static inline BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xQueue;
    (void)pvItemToQueue;
    (void)pxHigherPriorityTaskWoken;
    return pdTRUE;
}

static inline BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void *pvBuffer, BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xQueue;
    (void)pvBuffer;
    (void)pxHigherPriorityTaskWoken;
    return pdTRUE;
}

static inline void vQueueDelete(QueueHandle_t xQueue) {
    (void)xQueue;
}

static inline BaseType_t xQueueIsQueueEmptyFromISR(QueueHandle_t xQueue) {
    (void)xQueue;
    return pdTRUE;
}

static inline BaseType_t xQueueIsQueueFullFromISR(QueueHandle_t xQueue) {
    (void)xQueue;
    return pdFALSE;
}

#endif /* NATIVE_BUILD */

#endif /* FREERTOS_SEMPHR_H */
