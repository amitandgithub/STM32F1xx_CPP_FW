/*
 * Queue.hpp
 *
 *  Created on: 14-Sep-2018
 *      Author: Amit Chaudhary
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <cstdint>


namespace Utils
{
    
#define QDBG 0

template<typename T, uint32_t QSize = 10>
class Queue {
public:
    static const uint32_t Q_Size = QSize;
	typedef enum
	{
		QSUCESS         = 0,
		QFULL           = 1,
		QEMPTY          = 2,
		QNULLPOINTER    = 3,
	}QSTATUS;

	Queue();
	 ~Queue();
	QSTATUS Write(T data);
    QSTATUS WriteFront(T data);
	QSTATUS Read(T &data);
	inline bool IsQueueFull();
	inline bool IsQueueEmpty();
    inline uint32_t GetSize(){return QSize; }
    inline uint32_t Available(){return Q_Size - m_Available; }
private:
	T m_QueueMem[QSize];
	int32_t m_WriteHead;
	int32_t m_ReadHead;
    int32_t m_Available;
};



template<typename T, uint32_t QSize>
Queue<T,QSize>::Queue()
{

    /*
    for(uint32_t i=0;i<Q_Size;i++)
    {
        m_QueueMem[i] = 0;
    }
    */
    
	m_WriteHead = 0;
	m_ReadHead  = 0;
    m_Available = 0;

}
template<typename T, uint32_t QSize>
bool Queue<T, QSize>::IsQueueFull()
{
    if( ((m_WriteHead + 1) %  Q_Size) == m_ReadHead )
    {
#if QDBG
        printf("******Queue Full********\n");
#endif
        return 1;
    }
    else
    {
        return 0;
    }

}
template<typename T, uint32_t QSize>
bool Queue<T, QSize>::IsQueueEmpty()
{
	if(m_WriteHead == m_ReadHead)
	{
#if QDBG
        printf("******Queue Empty********\n");
#endif
		return true;
	}
	return false;

}

template<typename T, uint32_t QSize>
Queue<T, QSize>::QSTATUS Queue<T, QSize>::Write(T data )
{
#if QDBG
     printf("Write -> m_WriteHead(%d), m_ReadHead(%d)   m_Available(%d)\n",m_WriteHead,m_ReadHead,m_Available);
#endif
    if(IsQueueFull())
	{
		return QFULL;
	}
	else
	{
       m_QueueMem[m_WriteHead] = data;
       m_WriteHead = (m_WriteHead + 1) % Q_Size;
       m_Available++;
	}
	return QSUCESS;
}

template<typename T, uint32_t QSize>
Queue<T, QSize>::QSTATUS Queue<T, QSize>::WriteFront(T data )
{
#if QDBG
     printf("Write -> m_WriteHead(%d), m_ReadHead(%d)   m_Available(%d)\n",m_WriteHead,m_ReadHead,m_Available);
#endif
    if(IsQueueFull())
	{
		return QFULL;
	}
	else
	{
       m_QueueMem[m_ReadHead - 1] = data;
       m_ReadHead = (m_ReadHead - 1) % Q_Size;
       m_Available++;
	}
	return QSUCESS;
}

template<typename T, uint32_t QSize>
Queue<T, QSize>::QSTATUS Queue<T, QSize>::Read(T &data )
{
#if QDBG
    printf("Read -> m_WriteHead(%d), m_ReadHead(%d)   m_Available(%d)\n",m_WriteHead,m_ReadHead,m_Available);
#endif
    if(IsQueueEmpty())
	{
		return QEMPTY;
	}
    else
    {
        data = m_QueueMem[m_ReadHead];
        m_ReadHead = ( m_ReadHead + 1 ) % Q_Size;
        m_Available--;
    }

	return QSUCESS;
}
template<typename T, uint32_t QSize>
Queue<T, QSize>::Queue::~Queue()
{

}


}
#endif /* QUEUE_H */
