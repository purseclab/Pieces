#ifndef FREERTOS_CHECKED
#define FREERTOS_CHECKED
#include <rtmk.h>
#include <stdchecked.h>
#include "FreeRTOS.h"
#include <task.h>

#pragma CHECKED_SCOPE ON
#define xQueueHandle_t const ptr<struct QueueDefinition>
ptr<void> getObjectFromHandle(ptr<void> handle);
COMPAT void fail(void);
#define MAX_OBJECTS	2048
typedef struct ownership_map {
		ptr<void>	owner;
		ptr<void>	object;
} OMAP;

#define configMAX_TASK_NAME_LEN 2
#define configMAX_PARAM         1
#define configMAX_ITEM_QUEUE 4
typedef void SafeTaskFunction_t(ptr<void> param);
extern int used;
extern OMAP omap checked[MAX_OBJECTS];
/* TODO: The bounds information must come from something else */
COMPAT xQueueHandle_t SafeQueueCreate(UBaseType_t len, UBaseType_t size);
COMPAT BaseType_t SafeTaskCreate(ptr<SafeTaskFunction_t> pvTaskCode,
                       const char * const __attribute__ ((nonnull)) pcName : byte_count(configMAX_TASK_NAME_LEN),
                       configSTACK_DEPTH_TYPE usStackDepth,
                       void* __attribute__ ((nonnull,uservalue)) pvParameters: byte_count(configMAX_PARAM) ,
                       UBaseType_t uxPriority,
                       ptr<ptr<struct tskTaskControlBlock>>pxCreatedTask
                       );
COMPAT void SafeTaskSuspend( ptr<struct tskTaskControlBlock> xTaskToSuspend );
COMPAT  void SafeTaskPrioritySet( ptr<struct tskTaskControlBlock> xTask,
                           UBaseType_t uxNewPriority );
COMPAT void SafeTaskResume(ptr<struct tskTaskControlBlock> xTaskToSuspend);
COMPAT  eTaskState SafeeTaskGetState( ptr<struct tskTaskControlBlock> xTask );
COMPAT void SafeWrite (int count, nt_array_ptr<char> buf: byte_count(bufsize),int bufsize);
COMPAT void SafeTaskDelayUntil(array_ptr<TickType_t const> pxPreviousWakeTime,
                                const TickType_t xTimeIncrement );
COMPAT BaseType_t SafeQueueSend(_Ptr<struct QueueDefinition> xQueue,
                              const void * const pvItemToQueue : byte_count(configMAX_ITEM_QUEUE),
                    			TickType_t xTicksToWait);
COMPAT BaseType_t SafeQueueReceive(_Ptr<struct QueueDefinition> xQueue,
                              void * const pvBuffer : byte_count(configMAX_ITEM_QUEUE),
                                TickType_t xTicksToWait);
COMPAT BaseType_t SafeQueueGenericSend(_Ptr<struct QueueDefinition> xQueue,
                              const void * const pvItemToQueue : byte_count(configMAX_ITEM_QUEUE),
                              TickType_t xTicksToWait,
               				  const BaseType_t xCopyPosition );
COMPAT ptr<struct QueueDefinition> SafeQueueSelectFromSet(ptr<struct QueueDefinition> xQueueSet,
                                                TickType_t const xTicksToWait );
COMPAT xQueueHandle_t SafeQueueCreateSet( const UBaseType_t uxEventQueueLength );
COMPAT BaseType_t SafeQueueAddToSet( ptr<struct QueueDefinition> xQueueOrSemaphore,
                              ptr<struct QueueDefinition> xQueueSet );
COMPAT BaseType_t SafeQueueRemoveFromSet(ptr<struct QueueDefinition> xQueueOrSemaphore,
                                   ptr<struct QueueDefinition> xQueueSet );
COMPAT UBaseType_t SafeQueueMessagesWaiting( const ptr<struct QueueDefinition> xQueue );

COMPAT void SafeQueueDelete(_Ptr<struct QueueDefinition> xQueue );

COMPAT BaseType_t SafeQueuePeek(_Ptr<struct QueueDefinition>xQueue, ptr<void > pvBuffer, TickType_t xTicksToWait );

COMPAT _Itype_for_any(T)
BaseType_t SafeQueuePeekGen(_Ptr<struct QueueDefinition>xQueue, array_ptr<T> pvBuffer, TickType_t xTicksToWait );

COMPAT BaseType_t SafeQueueReceiveFromISR( ptr<struct QueueDefinition> xQueue,
                                 ptr<void> const pvBuffer,
                                 ptr<BaseType_t> const pxHigherPriorityTaskWoken );
COMPAT ptr<struct QueueDefinition> SafeQueueSelectFromSetFromISR( ptr<struct QueueDefinition> xQueueSet );
COMPAT BaseType_t SafeQueueGenericSendFromISR( ptr<struct QueueDefinition>  xQueue,
                                     ptr<const void > const pvItemToQueue,
                                     ptr<BaseType_t> const pxHigherPriorityTaskWoken,
                                     const BaseType_t xCopyPosition );
COMPAT xQueueHandle_t SafeQueueCreateMutex( const uint8_t ucQueueType );
COMPAT BaseType_t SafeQueueGiveMutexRecursive( _Ptr<struct QueueDefinition> xMutex );
COMPAT BaseType_t SafeQueueTakeMutexRecursive( _Ptr<struct QueueDefinition> xMutex,
                                         TickType_t xTicksToWait );
COMPAT void SafeQueueAddToRegistry(ptr<struct QueueDefinition> xQueue,
                              ptr<const char> pcQueueName );


#if configUSE_CO_ROUTINES > 0
typedef void SafecrCOROUTINE_CODE( CoRoutineHandle_t,
                                   UBaseType_t );
COMPAT BaseType_t SafexCoRoutineCreate( ptr<SafecrCOROUTINE_CODE> pxCoRoutineCode,
                             UBaseType_t uxPriority,
                             UBaseType_t uxIndex );
COMPAT int SafegetState(SafeCoRoutineHandle_t handle);
COMPAT BaseType_t SafexQueueCRSend(_Ptr<struct QueueDefinition> xQueue,
                            ptr<const void> pvItemToQueue,
                             TickType_t xTicksToWait );
COMPAT BaseType_t SafexQueueCRReceive(_Ptr<struct QueueDefinition> xQueue,
                               ptr<const void> pvItemToQueue,
                               TickType_t xTicksToWait );
COMPAT void SafecrSET_STATE(SafeCoRoutineHandle_t handle,int line);
#endif

COMPAT ptr<struct StreamBufferDef_t> SafeStreamBufferGenericCreate( size_t xBufferSizeBytes,
                                                     size_t xTriggerLevelBytes,
                                                     BaseType_t xIsMessageBuffer );
COMPAT size_t SafeStreamBufferSend( ptr<struct StreamBufferDef_t> xStreamBuffer,
                          ptr<const void> pvTxData,
                          size_t xDataLengthBytes,
                          TickType_t xTicksToWait );
COMPAT size_t SafeStreamBufferReceive( ptr<struct StreamBufferDef_t> xStreamBuffer,
                             ptr<void> pvRxData,
                             size_t xBufferLengthBytes,
                             TickType_t xTicksToWait );

int safe_strlen(ptr<char> input_ptr);
//util functions
int mymemcmp ( ptr<const void> ptr1, ptr<const void> ptr2, size_t num );
unsigned int mystrlen(ptr<const char>str);
unsigned long long curr_stamp(void);
void write_number(unsigned long long  stmp);

#endif /* "FREERTOS_CHECKED" */
