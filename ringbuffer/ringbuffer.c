/**
************************************************************
* @file         ringbuffer.c
* @brief        环形FIFO缓冲区
* @author       YM
* @date         2020-07-02
* @version      V0.1
*
* @note         使用ringbuffer_t 结构体将一段连续内存空间（静态分配的全局数组）
*               组织成FIFO环形缓冲区。数据从Tail写入，从Head读出。操作的都是地址   
************************************************************
*/

/*
1.Head < Tail
    warning：Head & Tail 是地址，而rbSize是字节，只有Head和Tail运算的结果(地址偏移)才可以参与rbSize的运算

    CanReadSize = Tail - Head
    CanWriteSize = rbSize - (Tail - Head)
    tailAvailableSize = rbSize - (Tail - rbBuff[begin])
     _____________________________________
    |              |            |         |
    |   empty      |    data    |  empty  |
    |______________|____________|_________|
rbBuff[begin]    Head          Tail     rbBuff[end]


2.Head > Tail
    CanReadSize = rbSize - (Head - Tail)
    CanWriteSize = Head - Tail
     _____________________________________
    |              |            |         |
    |     data     |    empty   |  data   |
    |______________|____________|_________|
rbBuff[begin]     Tail         Head     rbBuff[end]

*/


#include "ringbuffer.h"

/**
* @brief ring buffer initialize
*
* @param [in] rb           : Input ring buffer address
* @param [in] databuff     : Input data buff address
* @param [in] size         : Input data buff length
*
* @return : 0:Initialize sucess ;-1: failure;
*/
int8_t RingBuffer_Init(ringbuffer_t *rb, uint8_t *databuff, uint32_t size)
{
    if(NULL == rb)
    {
        return -1;
    }

    rb->rbBuff = databuff;
    rb->rbSize = size;
    rb->rbHead = databuff;
    rb->rbTail = databuff;

    return 0;
}

/**
* @brief  Delete ring buffer 
*
* @param [in] rb           : Input ring buffer address
*
* @return : 0:Delete sucess ;-1: failure;
*/
int8_t RingBuffer_Delete(ringbuffer_t *rb)
{
    if(NULL == rb)
    {
        return -1;
    }

    rb->rbBuff = NULL;
    rb->rbSize = 0;
    rb->rbHead = NULL;
    rb->rbTail = NULL;

    return 0;

}

/**
* @brief  Get the size of ring buffer
*
* @param [in] rb           : Input ring buffer address
*
* @return : -1:failure; else return ringbuffer end address
*/
int32_t RingBuffer_Get_rbSize(ringbuffer_t *rb)
{
    if(NULL == rb)
    {
        return -1;
    } 

    return rb->rbSize;
}


/**
* @brief Get the remaining readable size of ring buffer
*
* @param [in] rb           : Input ring buffer address
*
* @return : -1: failure;
*/
int32_t RingBuffer_Get_CanReadSize(ringbuffer_t *rb)
{
    if(NULL == rb)
    {
        return -1;
    } 
    if(rb->rbHead == rb->rbTail) //Head = Tail means ringbuffer empty
    {
        return 0;
    }

    if(rb->rbHead < rb->rbTail)
    {
        return (rb->rbTail - rb->rbHead);
    }
   
    return (RingBuffer_Get_rbSize(rb) - (rb->rbHead - rb->rbTail));
}


/**
* @brief Get the remaining writable size of ring buffer
*
* @param [in] rb           : Input ring buffer address
*
* @return : -1: failure;
*/
int32_t RingBuffer_Get_CanWriteSize(ringbuffer_t *rb)
{
    if(NULL == rb)
    {
        return -1;
    } 
   
    return (RingBuffer_Get_rbSize(rb)- RingBuffer_Get_CanReadSize(rb));
}


/**
* @brief Read ring buffer form Head
*
* @param [in]  rb           : Input ring buffer address
* @param [out] databuff     : Address of data storage
* @param [in]  size         : Length to read
*
* @return : Return read size ;-1: failure;
*/
int32_t RingBuffer_Read(ringbuffer_t *rb,void *databuff,uint8_t size)
{
    int32_t copysize = 0;

    if((NULL == rb) || (NULL == databuff))
    {
        return -1;
    } 

    //情况1
    if(rb->rbHead < rb->rbTail)
    {
        copysize = _MIN(size,RingBuffer_Get_CanReadSize(rb));//计算还可以读多少：可读的和要读的取最小。例如可读2字节，但是要读3字节，那么只能读2字节
        memcpy(databuff,rb->rbHead,copysize);//以内存复制方式，效率高
        rb->rbHead += copysize;//每读一次，Head加读取的字节，也就是头指针移位，空出位置给Tail写入
        return copysize;//返回读取的字节数

    }
    else //情况2 
    {
        //要读取的大小小于右边剩余的大小，那么直接读就可以
        if(size < RingBuffer_Get_rbSize(rb) - (rb->rbHead - rb->rbBuff))
        {
            copysize = size;
            memcpy(databuff,rb->rbHead ,copysize);
            rb->rbHead += copysize;
            return copysize;
        }
        else //要读取的大小比右边的大，那么先读右边剩下的大小，再去读左边剩下的
        {
            copysize = RingBuffer_Get_rbSize(rb) - (rb->rbHead - rb->rbBuff);
            memcpy(databuff,rb->rbHead,copysize);
            rb->rbHead = rb->rbBuff;
            copysize +=  RingBuffer_Read(rb,(char *)databuff + copysize,size - copysize );//(char *)是强制类型转换；将剩下的字节读取到databuff + copysize地址处
            return copysize;
        }
        
    }
    
}

/**
* @brief Write ring buffer form Tail
*
* @param [in]  rb           : Input ring buffer address
* @param [out] databuff     : Address of data buff to write
* @param [in]  size         : Length to write
*
* @return : Return write size ;-1: failure;-2: ring buffer not enough space
*/
int32_t RingBuffer_Write(ringbuffer_t *rb,void *databuff,uint8_t size)
{
    int32_t tailAvailableSize = 0;

    if((NULL == rb) || (NULL == databuff))
    {
        return -1;
    } 

    if(size > RingBuffer_Get_CanWriteSize(rb)) //如果要写入的大小 > 可写的大小，说明空间不够了，写入失败
    {
        return -2;
    }
    //情况1
    if(rb->rbHead <= rb->rbTail)
    {
        tailAvailableSize = RingBuffer_Get_rbSize(rb) - (rb->rbTail - rb->rbBuff);//获取右端剩余字节大小
        if(size < tailAvailableSize) //如果要写的 < 右边剩余的，直接写入右边
        {
            memcpy(rb->rbTail,databuff,size);
            rb->rbTail += size;
            if(rb->rbTail == (rb->rbBuff + RingBuffer_Get_rbSize(rb))) //如果写到ringbuffer结尾了，要重新回到开头去重新开始新的循环
            {
                rb->rbTail = rb->rbBuff;
            }
            return size;
        }
        else
        {
            memcpy(rb->rbTail,databuff,tailAvailableSize);//要写的 > 右边，先把右边的写完
            rb->rbTail = rb->rbBuff;//回到开头

            //重新写剩下的
            return (tailAvailableSize + RingBuffer_Write(rb,(char *)databuff + tailAvailableSize,size - tailAvailableSize));
        }
        
    }
    else //情况2，直接写
    {
        memcpy(rb->rbTail,databuff,size);
        rb->rbTail += size;

        return size;
    }
    
}

