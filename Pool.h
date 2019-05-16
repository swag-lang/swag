#pragma once
struct PoolElement
{
	PoolElement* nextFree;
	virtual void reset() = 0;
};

template<typename T, int S>
struct PoolSlot
{
	T			buffer[S];
	int			maxUsed = 1;
	PoolSlot*	nextSlot = nullptr;
};

template<typename T, int S = 512>
class Pool
{
private:
	PoolSlot<T, S>*	m_rootSlot = nullptr;
	PoolSlot<T, S>*	m_lastSlot = nullptr;
	PoolElement* m_firstFree   = nullptr;

public:
	T* alloc()
	{
		if (m_firstFree)
		{
			auto cur = m_firstFree;
			m_firstFree = m_firstFree->nextFree;
			cur->reset();
			return static_cast<T*>(cur);
		}

		if (!m_rootSlot)
		{
			m_rootSlot = m_lastSlot = new PoolSlot<T, S>();
			return &m_rootSlot->buffer[0];
		}

		if (m_lastSlot->maxUsed == S)
		{
			m_lastSlot->nextSlot = new PoolSlot<T, S>();
			m_lastSlot = m_lastSlot->nextSlot;
			return &m_lastSlot->buffer[0];
		}

		return &m_lastSlot->buffer[m_lastSlot->maxUsed++];
	}

	void free(T* addr)
	{
		addr->nextFree = m_firstFree;
		m_firstFree = addr;
	}
};

