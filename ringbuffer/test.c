#include "ringbuffer.h"

#define BUFF_LENGTH 10

uint8_t rbBuff[BUFF_LENGTH];

ringbuffer_t ringBuffer;


void main()
{
    int8_t flag = 0;
    int32_t wSize = 0;
    int32_t rSize = 0;
    uint8_t tmpData = 0;

    flag = RingBuffer_Init(&ringBuffer,rbBuff,sizeof(rbBuff));
    if(0 == flag)
    {
        printf("Ring buffer initialize sucess!\n");
    }
    else
    {
        printf("Error!Initialize failure!\n");
    }

    wSize = RingBuffer_Write(&ringBuffer,"123456789",9);//这里的是字符串的值

    if(-1 == wSize)
    {
        printf("Wtire ring buffer failure!\n");
    }
    else if(-2 == wSize)
    {
        printf("Ring buffer not enough space!\n");
    }
    else
    {
        printf("Write sucess and write size = %d\n",wSize);
    }

    rSize = RingBuffer_Get_CanReadSize(&ringBuffer);//读取当前有多少数据可读
    if(0 == rSize)
    {
        printf("Ring buffer is empty!\n");
    }

    for(uint8_t i=0; i<rSize; i++)
    {
       flag = RingBuffer_Read(&ringBuffer, &tmpData, 1);
       if(-1 == flag)
       {
           printf("Error!Fail to read!\n");
       }
       else
       {
           printf("Sucess! Read size = %d\n",flag);
       }
       
       printf("Read data is %d\n",tmpData);
    }

    


    




}



