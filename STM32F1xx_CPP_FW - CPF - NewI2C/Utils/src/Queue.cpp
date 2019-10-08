/*
 * Queue.cpp
 *
 *  Created on: 14-Sep-2018
 *      Author: Amit Chaudhary
 */

#include "Queue.h"

namespace Utils
{

template<typename T, uint32_t QSize>
Queue<T,QSize>::Queue()
{

    for(uint32_t i=0;i<Q_Size;i++)
    {
        m_QueueMem[i] = 0;
    }
    
	m_WriteHead = -1;
	m_ReadHead  = -1;

}
template<typename T, uint32_t QSize>
bool Queue<T, QSize>::IsQueueFull()
{
	if( ( (m_WriteHead == 0) && 
          (m_ReadHead == (signed)(Q_Size-1) )) || (m_ReadHead == (m_WriteHead-1)) 
      )
	{
		return true;
	}
	else
	{
		return false;
	}

}
template<typename T, uint32_t QSize>
bool Queue<T, QSize>::IsQueueEmpty()
{
	if(m_WriteHead == -1)
	{
		return true;
	}
	return false;

}

template<typename T, uint32_t QSize>
Queue<T, QSize>::QSTATUS Queue<T, QSize>::Write(T data )
{
	if(m_QueueMem == nullptr)
	{
		return QNULLPOINTER;
	}
	else if(IsQueueFull())
	{
		return QFULL;
	}
	else
	{
		if(m_ReadHead == -1)
			m_ReadHead = 0;

		m_WriteHead = (m_WriteHead + 1) % Q_Size;

		m_QueueMem[m_WriteHead] = data;
	}
	return QSUCESS;
}
template<typename T, uint32_t QSize>
Queue<T, QSize>::QSTATUS Queue<T, QSize>::Read(T &data )
{
    if(IsQueueEmpty())
	{
		return QEMPTY;
	}
	else
	{
		data = m_QueueMem[m_WriteHead];

		if(m_ReadHead == m_WriteHead)
		{
			// Q has only one element, so we reset the queue
			m_ReadHead = m_WriteHead = -1;
		}
		else
		{
			m_ReadHead = (m_ReadHead + 1) % Q_Size;
		}
	}
	return QSUCESS;
}
template<typename T, uint32_t QSize>
Queue<T, QSize>::Queue::~Queue()
{

}


}// namespace Bsp
