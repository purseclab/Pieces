#include <stdchecked.h>
#include <FreeRTOS.h>
#include <rtmk.h>
#include <task.h>
#include <queue.h>
#include <stdio.h>
#include <stream_buffer.h>
#include "semphr.h"
#include "croutine.h"

typedef ptr<void> SafeCoRoutineHandle_t;
typedef void SafecrCOROUTINE_CODE( CoRoutineHandle_t,
                                   UBaseType_t );
#define xQueueHandle_t const ptr<struct QueueDefinition>
#define MAX_OBJECTS 2048
typedef struct ownership_map {
        ptr<void>   owner;
        ptr<void>   object;
} OMAP;

#define configMAX_TASK_NAME_LEN 2
#define configMAX_PARAM         1
#define configMAX_ITEM_QUEUE 4
typedef void SafeTaskFunction_t(ptr<void> param);
int used;
OMAP omap checked[MAX_OBJECTS];

COMPAT
int safe_strlen(ptr<char> input_ptr) {
//TODO:
		return 0;
} 
ptr<void> getObjectFromHandle(ptr<void> handle) {
		unchecked {
				ptr<OMAP> elem = _Assume_bounds_cast<ptr<OMAP>> (handle);
				return elem->object;
		}
}
COMPAT
void fail(void ) {
		while(1);
}

/* TODO: The bounds information must come from something else */
COMPAT
xQueueHandle_t SafeQueueCreate(UBaseType_t len, UBaseType_t size) {
		ptr<struct QueueDefinition> ret= NULL;
		unchecked {
				ret = _Assume_bounds_cast<xQueueHandle_t>(xQueueCreate(len,size));
				if (ret && used < MAX_OBJECTS) {
					omap[used].owner = _Assume_bounds_cast<ptr<void>> (xTaskGetCurrentTaskHandle());
					omap[used].object = ret;
					ret = _Assume_bounds_cast<xQueueHandle_t> (&omap[used].owner);
					used ++;
				}
		}
		if (ret) {
		}
		else {
				fail();
		}
		return ret;
}

COMPAT
BaseType_t SafeTaskCreate(ptr<SafeTaskFunction_t> pvTaskCode,
                       const char * const __attribute__ ((nonnull)) pcName : byte_count(configMAX_TASK_NAME_LEN),
                       configSTACK_DEPTH_TYPE usStackDepth,
                       void* __attribute__ ((nonnull,uservalue)) pvParameters: byte_count(configMAX_PARAM) ,
                       UBaseType_t uxPriority,
                       ptr<ptr<struct tskTaskControlBlock>>pxCreatedTask
                       ) {
		BaseType_t ret = 0;
		unchecked {
		 	struct tskTaskControlBlock * pxCreatedTaskLocal;
			ret = xTaskCreate((TaskFunction_t)pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, (struct tskTaskControlBlock **)&pxCreatedTaskLocal);

			if (ret && used < MAX_OBJECTS) {
                    omap[used].owner = _Assume_bounds_cast<ptr<void>> (xTaskGetCurrentTaskHandle());
                    omap[used].object =_Assume_bounds_cast<ptr<void>> (pxCreatedTaskLocal);
					if (pxCreatedTask)
							*pxCreatedTask = _Assume_bounds_cast<ptr<struct tskTaskControlBlock>> (pxCreatedTaskLocal);
                    used ++;
            }
		}
		return ret;
}

COMPAT
void SafeTaskSuspend( ptr<struct tskTaskControlBlock> xTaskToSuspend ) {
		unchecked {
				TaskHandle_t t =(TaskHandle_t ) getObjectFromHandle((void *) xTaskToSuspend);
				vTaskSuspend(t);
		}
}

COMPAT
void SafeTaskPrioritySet( ptr<struct tskTaskControlBlock> xTask,
                           UBaseType_t uxNewPriority ) {
		unchecked {
				TaskHandle_t t =(TaskHandle_t ) getObjectFromHandle((void *) xTask);
				vTaskPrioritySet(t, uxNewPriority);
		}
}

COMPAT
void SafeTaskResume(ptr<struct tskTaskControlBlock> xTaskToSuspend) {
		unchecked {
				TaskHandle_t t =(TaskHandle_t ) getObjectFromHandle((void *) xTaskToSuspend);
                vTaskResume(t);
        }
}

COMPAT
eTaskState SafeeTaskGetState( ptr<struct tskTaskControlBlock> xTask ) {
		eTaskState ret; 
		unchecked {
				TaskHandle_t t =(TaskHandle_t ) getObjectFromHandle((void *) xTask);
				ret = eTaskGetState(t);
		}
		return ret;
}



COMPAT
void SafeWrite (int count, nt_array_ptr<char> buf: byte_count(bufsize),int bufsize) {
	unchecked {
		_write(count, buf, bufsize);
	}
}


COMPAT
void SafeTaskDelayUntil(array_ptr<TickType_t const> pxPreviousWakeTime,
                                const TickType_t xTimeIncrement ) {
		BaseType_t ret; 
		unchecked {
			ret = xTaskDelayUntil((TickType_t * const)pxPreviousWakeTime, xTimeIncrement);
		}
		if (ret!= pdTRUE) {
				fail();
		}
}


COMPAT
BaseType_t SafeQueueSend(_Ptr<struct QueueDefinition> xQueue,
                              const void * const pvItemToQueue : byte_count(configMAX_ITEM_QUEUE),
                    			TickType_t xTicksToWait) {
		BaseType_t ret;
		unchecked {
				QueueHandle_t q =(QueueHandle_t ) getObjectFromHandle((void *) xQueue);
			ret = xQueueSend(q, pvItemToQueue, xTicksToWait);
		}
		return ret;
}

COMPAT
BaseType_t SafeQueueReceive(_Ptr<struct QueueDefinition> xQueue,
                              void * const pvBuffer : byte_count(configMAX_ITEM_QUEUE),
                                TickType_t xTicksToWait) {
        BaseType_t ret;
        unchecked {
			QueueHandle_t q =(QueueHandle_t ) getObjectFromHandle((void *) xQueue);
            ret = xQueueReceive(q, pvBuffer, xTicksToWait);
        }
        return ret;
}

COMPAT BaseType_t SafeQueueGenericSend(_Ptr<struct QueueDefinition> xQueue,
                              const void * const pvItemToQueue : byte_count(configMAX_ITEM_QUEUE),
                              TickType_t xTicksToWait,
               				  const BaseType_t xCopyPosition ) {
		BaseType_t ret;
		unchecked {
				QueueHandle_t q =(QueueHandle_t ) getObjectFromHandle((void *) xQueue);
				ret = xQueueGenericSend(q, pvItemToQueue, xTicksToWait, xCopyPosition);
		}
		return ret;

}
COMPAT ptr<struct QueueDefinition> SafeQueueSelectFromSet(ptr<struct QueueDefinition> xQueueSet,
                                                TickType_t const xTicksToWait ) {
		ptr<struct QueueDefinition> ret = NULL;
		unchecked {
			QueueSetHandle_t qset  = (QueueSetHandle_t)getObjectFromHandle((void *)xQueueSet);
			ret = assume_bounds_cast<ptr<struct QueueDefinition>> (xQueueSelectFromSet(qset, xTicksToWait ));
			ret = assume_bounds_cast<ptr<struct QueueDefinition>> (getObjectFromHandle(ret));
		}
		return ret;
}

COMPAT xQueueHandle_t SafeQueueCreateSet( const UBaseType_t uxEventQueueLength ) {
		_Ptr<struct QueueDefinition> ret = NULL;
		unchecked {
			ret = assume_bounds_cast<ptr<struct QueueDefinition>> (xQueueCreateSet(uxEventQueueLength));
            if (ret && used < MAX_OBJECTS) {
                    omap[used].owner = _Assume_bounds_cast<ptr<void>> (xTaskGetCurrentTaskHandle());
                    omap[used].object = ret;
                    ret = _Assume_bounds_cast<xQueueHandle_t> (&omap[used].owner);
                    used ++;
            }
		}
		return ret;
}

COMPAT BaseType_t SafeQueueAddToSet( ptr<struct QueueDefinition> xQueueOrSemaphore,
                              ptr<struct QueueDefinition> xQueueSet ) {
		BaseType_t ret;
		unchecked {
			QueueSetMemberHandle_t q = (QueueSetMemberHandle_t)getObjectFromHandle((void *) xQueueOrSemaphore);
			QueueSetHandle_t qset  = (QueueSetHandle_t)getObjectFromHandle((void *)xQueueSet);
			ret = xQueueAddToSet(q, qset);
		}
		return ret;
}

COMPAT BaseType_t SafeQueueRemoveFromSet(ptr<struct QueueDefinition> xQueueOrSemaphore,
                                   ptr<struct QueueDefinition> xQueueSet ) {
		BaseType_t ret;
		unchecked {
				QueueSetMemberHandle_t q = (QueueSetMemberHandle_t)getObjectFromHandle((void *) xQueueOrSemaphore);
	            QueueSetHandle_t qset  = (QueueSetHandle_t)getObjectFromHandle((void *)xQueueSet);
				ret = xQueueRemoveFromSet(q,
                               qset);
		}
		return ret;
}

COMPAT UBaseType_t SafeQueueMessagesWaiting( const ptr<struct QueueDefinition> xQueue ) {
		UBaseType_t ret;
		unchecked {
				QueueHandle_t q =(QueueHandle_t ) getObjectFromHandle((void *) xQueue);
				ret =uxQueueMessagesWaiting(q);
		}
		return ret;
}

COMPAT void SafeQueueDelete(_Ptr<struct QueueDefinition> xQueue ) {
		unchecked {
				QueueHandle_t q = (QueueHandle_t ) getObjectFromHandle((void *) xQueue);
				vQueueDelete(q);
		}
}

COMPAT BaseType_t SafeQueuePeek(_Ptr<struct QueueDefinition>xQueue, ptr<void > pvBuffer, TickType_t xTicksToWait ) {
		BaseType_t ret;
		unchecked {
				QueueHandle_t q =(QueueHandle_t )getObjectFromHandle((void *) xQueue);
				ret = xQueuePeek(q, (void *)pvBuffer, xTicksToWait);
		}
		return ret;
}

COMPAT _Itype_for_any(T)
BaseType_t SafeQueuePeekGen(_Ptr<struct QueueDefinition>xQueue, array_ptr<T> pvBuffer, TickType_t xTicksToWait ) {
		BaseType_t ret;
        unchecked {
				QueueHandle_t q =(QueueHandle_t ) getObjectFromHandle((void *) xQueue);
                ret = xQueuePeek(q, (void *)pvBuffer, xTicksToWait);
        }
        return ret;
}

COMPAT BaseType_t SafeQueueReceiveFromISR( ptr<struct QueueDefinition> xQueue,
                                 ptr<void> const pvBuffer,
                                 ptr<BaseType_t> const pxHigherPriorityTaskWoken ) {
		BaseType_t ret;
		unchecked{
				QueueHandle_t q = (QueueHandle_t ) getObjectFromHandle((void *) xQueue);
				ret = xQueueReceiveFromISR(q,(void * const) pvBuffer, (BaseType_t *)pxHigherPriorityTaskWoken);
		}
		return ret;
}

COMPAT ptr<struct QueueDefinition> SafeQueueSelectFromSetFromISR( ptr<struct QueueDefinition> xQueueSet ) {
		ptr<struct QueueDefinition> ret = NULL;
		unchecked {
				QueueHandle_t qset = (QueueHandle_t )getObjectFromHandle((void *) xQueueSet);
				ret = assume_bounds_cast<ptr<struct QueueDefinition>> (xQueueSelectFromSetFromISR(qset));
		}
		return ret;
}
COMPAT BaseType_t SafeQueueGenericSendFromISR( ptr<struct QueueDefinition>  xQueue,
                                     ptr<const void > const pvItemToQueue,
                                     ptr<BaseType_t> const pxHigherPriorityTaskWoken,
                                     const BaseType_t xCopyPosition ) {
		BaseType_t ret;
		unchecked {
				QueueHandle_t q =(QueueHandle_t ) getObjectFromHandle((void *) xQueue);
				ret = xQueueGenericSendFromISR(q,
                                     (const void * const) pvItemToQueue,
                                     (BaseType_t * const) pxHigherPriorityTaskWoken,
                                     xCopyPosition );
		}
		return ret;
}
COMPAT xQueueHandle_t SafeQueueCreateMutex( const uint8_t ucQueueType ) {
		_Ptr<struct QueueDefinition> ret = NULL;
		unchecked {
				ret = assume_bounds_cast<_Ptr<struct QueueDefinition>> (xQueueCreateMutex(ucQueueType));

				if (ret && used < MAX_OBJECTS) {
                    omap[used].owner = _Assume_bounds_cast<ptr<void>> (xTaskGetCurrentTaskHandle());
                    omap[used].object = ret;
                    ret = _Assume_bounds_cast<xQueueHandle_t> (&omap[used].owner);
                    used ++;
            	}
		}
		return ret;
}

COMPAT BaseType_t SafeQueueGiveMutexRecursive( _Ptr<struct QueueDefinition> xMutex ) {
		BaseType_t ret;
		unchecked {
			QueueHandle_t m =(QueueHandle_t ) getObjectFromHandle((void *) xMutex);
			ret = xQueueGiveMutexRecursive((QueueHandle_t) m);
		}
		return ret;
}
COMPAT BaseType_t SafeQueueTakeMutexRecursive( _Ptr<struct QueueDefinition> xMutex,
                                         TickType_t xTicksToWait ) {
		BaseType_t ret;
		unchecked {
				QueueHandle_t m = (QueueHandle_t )getObjectFromHandle((void *) xMutex);
				ret = xQueueTakeMutexRecursive(m, xTicksToWait);
		}
		return ret;
}
COMPAT void SafeQueueAddToRegistry(ptr<struct QueueDefinition> xQueue,
                              ptr<const char> pcQueueName ) {
		unchecked {
				QueueHandle_t q =(QueueHandle_t ) getObjectFromHandle((void *) xQueue);
				vQueueAddToRegistry(q, (char *)pcQueueName);
		}
}

#if configUSE_CO_ROUTINES > 0
COMPAT BaseType_t SafexCoRoutineCreate( ptr<SafecrCOROUTINE_CODE> pxCoRoutineCode,
                             UBaseType_t uxPriority,
                             UBaseType_t uxIndex ) {
		unchecked {
			return xCoRoutineCreate( (crCOROUTINE_CODE)pxCoRoutineCode, uxPriority, uxIndex);
		}
}
COMPAT int SafegetState(SafeCoRoutineHandle_t handle) {
		unchecked {
				return getState((void *)handle);
		}
}

COMPAT BaseType_t SafexQueueCRSend(_Ptr<struct QueueDefinition> xQueue,
                            ptr<const void> pvItemToQueue,
                             TickType_t xTicksToWait ) {
		unchecked {
				return xQueueCRSend((QueueHandle_t)xQueue, (void *)pvItemToQueue, xTicksToWait);
		}
}

COMPAT BaseType_t SafexQueueCRReceive(_Ptr<struct QueueDefinition> xQueue,
                               ptr<const void> pvItemToQueue,
                               TickType_t xTicksToWait ) {
        unchecked {
                return xQueueCRReceive((QueueHandle_t)xQueue, (void *)pvItemToQueue, xTicksToWait);
        }
}
COMPAT void SafecrSET_STATE(SafeCoRoutineHandle_t handle,int line) {
		unchecked {
				crSET_STATE0_inline((void * ) handle, line);
		}
}
#endif 
COMPAT
ptr<struct StreamBufferDef_t> SafeStreamBufferGenericCreate( size_t xBufferSizeBytes,
                                                     size_t xTriggerLevelBytes,
                                                     BaseType_t xIsMessageBuffer ) {
		ptr<struct StreamBufferDef_t> ret = NULL;
		unchecked {
				ret= assume_bounds_cast<ptr<struct StreamBufferDef_t>> (xStreamBufferGenericCreate(xBufferSizeBytes, xTriggerLevelBytes, xIsMessageBuffer));

				if (ret && used < MAX_OBJECTS) {
                    omap[used].owner = _Assume_bounds_cast<ptr<void>> (xTaskGetCurrentTaskHandle());
                    omap[used].object = ret;
                    ret = _Assume_bounds_cast<ptr<struct StreamBufferDef_t>> (&omap[used].owner);
                    used ++;
                }
		}
		return ret;
}

COMPAT
size_t SafeStreamBufferSend( ptr<struct StreamBufferDef_t> xStreamBuffer,
                          ptr<const void> pvTxData,
                          size_t xDataLengthBytes,
                          TickType_t xTicksToWait ) {
		size_t ret;
		unchecked {
			StreamBufferHandle_t  arg =(StreamBufferHandle_t) getObjectFromHandle((void *) xStreamBuffer);
			ret = xStreamBufferSend(arg, (void *)pvTxData,xDataLengthBytes,xTicksToWait);


		}
		return ret;
}

COMPAT
size_t SafeStreamBufferReceive( ptr<struct StreamBufferDef_t> xStreamBuffer,
                             ptr<void> pvRxData,
                             size_t xBufferLengthBytes,
                             TickType_t xTicksToWait ) {
		size_t ret; 
		unchecked {
				StreamBufferHandle_t  arg =(StreamBufferHandle_t) getObjectFromHandle((void *) xStreamBuffer);
				ret = xStreamBufferReceive(arg,
                             (void *)pvRxData,
                             xBufferLengthBytes,
                             xTicksToWait );
		}
		return ret;
}


