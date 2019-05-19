#pragma once
struct IPool
{
	virtual void free(void *addr) = 0;
};

struct PoolElement
{
	PoolElement* nextFree = nullptr;
	IPool* m_pool = nullptr;
	void release() { if(m_pool) m_pool->free(this); }
	virtual void reset() = 0;
	virtual void construct() = 0;
};

template<typename T, int S>
struct PoolSlot
{
	T			buffer[S];
	int			maxUsed = 1;
	PoolSlot*	nextSlot = nullptr;
};


template<typename T, int S = 1024>
class Pool : public IPool
{
private:
	PoolSlot<T, S>*	m_rootSlot = nullptr;
	PoolSlot<T, S>*	m_lastSlot = nullptr;
	PoolElement* m_firstFree   = nullptr;
	mutex m_mutex;

public:
	T* alloc()
	{
		lock_guard<mutex> lg(m_mutex);
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
			auto elem = &m_lastSlot->buffer[0];
			elem->construct();
			elem->m_pool = this;
			return elem;
		}

		if (m_lastSlot->maxUsed == S)
		{
			m_lastSlot->nextSlot = new PoolSlot<T, S>();
			m_lastSlot = m_lastSlot->nextSlot;
			auto elem = &m_lastSlot->buffer[0];
			elem->m_pool = this;
			elem->construct();
			return elem;
		}

		auto elem = &m_lastSlot->buffer[m_lastSlot->maxUsed++];
		elem->m_pool = this;
		elem->construct();
		return elem;
	}

	void free(void* addr)
	{
		lock_guard<mutex> lg(m_mutex);
		((T*) addr)->nextFree = m_firstFree;
		m_firstFree = ((T*)addr);
	}
};
