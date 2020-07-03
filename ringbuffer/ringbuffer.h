#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define _MIN(a,b)    ((a)<(b))?(a):(b)


#pragma pack(1)  //1字节对齐
typedef struct 
{
    uint32_t rbSize; //buffer size
    uint8_t *rbBuff; //Managed buffer
    uint8_t *rbHead; //Read point
    uint8_t *rbTail; //Write point  

}ringbuffer_t;
#pragma pack()

int8_t RingBuffer_Init(ringbuffer_t *rb, uint8_t *databuff, uint32_t size);
int8_t RingBuffer_Delete(ringbuffer_t *rb);
int32_t RingBuffer_Get_rbSize(ringbuffer_t *rb);
int32_t RingBuffer_Get_CanReadSize(ringbuffer_t *rb);
int32_t RingBuffer_Get_CanWriteSize(ringbuffer_t *rb);
int32_t RingBuffer_Read(ringbuffer_t *rb,void *databuff,uint8_t size);
int32_t RingBuffer_Write(ringbuffer_t *rb,void *databuff,uint8_t size);

#ifdef __cplusplus
}
#endif

#endif

