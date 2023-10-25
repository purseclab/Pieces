#define xQueueCreate(len,size) SafeQueueCreate(len,size)
#define TaskFunction_t SafeTaskFunction_t
#define TaskHandle_t ptr<struct tskTaskControlBlock>
#define xTaskCreate(task,name,stack,param,pri,taskh) SafeTaskCreate(task,name,stack,param,pri,taskh)
#define vTaskSuspend(taskh)     SafeTaskSuspend(taskh)
#define vTaskResume(taskh)      SafeTaskResume(taskh)
#define eTaskGetState(taskh)    SafeeTaskGetState(taskh)
#define vTaskPrioritySet(taskh,prio)    SafeTaskPrioritySet(taskh,prio)
#define _write(a,b,c) SafeWrite(a,b,c)
#define vTaskDelayUntil( pxPreviousWakeTime, xTimeIncrement ) ( pxPreviousWakeTime, xTimeIncrement )
#define xCoRoutineCreate(a,b,c) SafexCoRoutineCreate(a,b,c) 
#define vQueueAddToRegistry(q,name) SafeQueueAddToRegistry(q,name)
#define xSemaphoreCreateRecursiveMutex() SafeQueueCreateMutex(queueQUEUE_TYPE_RECURSIVE_MUTEX)
#define xSemaphoreGiveRecursive(mut) SafeQueueGiveMutexRecursive(mut)
#define xSemaphoreTakeRecursive(mut, tick)  SafeQueueTakeMutexRecursive(mut, tick)
#define xQueueOverwriteFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) \
    SafeQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueOVERWRITE )
#define xQueueSelectFromSetFromISR(q) SafeQueueSelectFromSetFromISR(q)
#define xQueueSendFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) \
        SafeQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_BACK )
#define vQueueDelete(xQueue)    SafeQueueDelete(xQueue)
#define uxQueueMessagesWaiting(xQueue) SafeQueueMessagesWaiting(xQueue)
#define xQueueSendToBack(q,value,delay) SafeQueueGenericSend(q,value,delay,queueSEND_TO_BACK)
#define xQueueOverwrite(q,value)  SafeQueueGenericSend(q,value,0, queueOVERWRITE)
#define xQueueReceive(q,buf,tick) SafeQueueReceive(q,buf,tick)
#define QueueHandle_t _Ptr<struct QueueDefinition>
#define QueueSetHandle_t ptr<struct QueueDefinition>
#define QueueSetMemberHandle_t ptr<struct QueueDefinition>
#define xQueueCreateSet(len) SafeQueueCreateSet(len)    
#define xQueueSelectFromSet(q,tick) SafeQueueSelectFromSet(q,tick)
#define xQueueAddToSet(q,qs) SafeQueueAddToSet(q,qs)
#define xQueueRemoveFromSet(q,qs) SafeQueueRemoveFromSet(q,qs)
#define xQueuePeek(q,buf,ticks) SafeQueuePeek(q,buf,ticks)
#define xQueueReceiveFromISR(q,buf,du) SafeQueueReceiveFromISR(q,buf,du)
#define SemaphoreHandle_t QueueHandle_t
#define QueueSetHandle_t _Ptr<struct QueueDefinition>
#define xStreamBufferCreate(bb,tb) SafeStreamBufferGenericCreate(bb,tb, pdFALSE)
#define StreamBufferHandle_t ptr<struct StreamBufferDef_t>
#define xStreamBufferSend(buf,data,len,tick) SafeStreamBufferSend(buf,data,len,tick)
#define xStreamBufferReceive(buf,data,len,tick) SafeStreamBufferReceive(buf,data,len,tick)
#define xQueueSend(q,item,ticks) SafeQueueSend(q,item,ticks)

#define xQueueCreate(len,size) SafeQueueCreate(len,size)
#define CoRoutineHandle_t SafeCoRoutineHandle_t


#undef crSTART
#undef crEND

#define crSTART( pxCRCB )                            \
    switch( SafegetState(pxCRCB)) { \
        case 0:

#define crEND(pxCRCB) }
#define crSET_STATE( xHandle, num) \
        SafecrSET_STATE(xHandle, num); return;\
        case ( num):

#undef crQUEUE_SEND
#define crQUEUE_SEND( xHandle, pxQueue, pvItemToQueue, xTicksToWait, pxResult )           \
    {                                                                                     \
        *( pxResult ) = SafexQueueCRSend( ( pxQueue ), ( pvItemToQueue ), ( xTicksToWait ) ); \
        if( *( pxResult ) == errQUEUE_BLOCKED )                                           \
        {                                                                                 \
            crSET_STATE( ( xHandle ) , __LINE__ * 2 );                                   \
            *pxResult = SafexQueueCRSend( ( pxQueue ), ( pvItemToQueue ), 0 );            \
        }                                                                                 \
        if( *pxResult == errQUEUE_YIELD )                                                 \
        {                                                                                 \
            crSET_STATE( ( xHandle ) ,( __LINE__ * 2 ) + 1 );                                                  \
            *pxResult = pdPASS;                                                           \
        }                                                                                 \
    }

#undef crQUEUE_RECEIVE
#define crQUEUE_RECEIVE( xHandle, pxQueue, pvBuffer, xTicksToWait, pxResult )           \
    {                                                                                   \
        *( pxResult ) = SafexQueueCRReceive( ( pxQueue ), ( pvBuffer ), ( xTicksToWait ) ); \
        if( *( pxResult ) == errQUEUE_BLOCKED )                                         \
        {                                                                               \
            crSET_STATE( ( xHandle ) , __LINE__ * 2);                                                \
            *( pxResult ) = SafexQueueCRReceive( ( pxQueue ), ( pvBuffer ), 0 );            \
        }                                                                               \
        if( *( pxResult ) == errQUEUE_YIELD )                                           \
        {                                                                               \
            crSET_STATE( ( xHandle ) ,( __LINE__ * 2 ) + 1 );                                                \
            *( pxResult ) = pdPASS;                                                     \
        }                                                                               \
    }



