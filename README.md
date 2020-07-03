#This a basic ringbuffer

##Ringbuffer strcut
typedef struct 
{
    uint32_t rbSize; //buffer size
    uint8_t *rbBuff; //Managed buffer
    uint8_t *rbHead; //Read point
    uint8_t *rbTail; //Write point  

}ringbuffer_t;

###Two cases when read and write
1.Head < Tail
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
