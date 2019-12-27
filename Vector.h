#pragma once
#include "Allocator.h"

template<typename T>
struct VectorNative
{
    int count     = 0;
    int allocated = 0;
    T*  buffer    = nullptr;

    size_t capacity() const
    {
        return allocated;
    }

    size_t size() const
    {
        return count;
    }

    void operator=(const vector<T>& other)
    {
        count = (int) other.size();
        if (allocated < count)
            reserve(count, false);
        memcpy(buffer, &other[0], count * sizeof(T));
    }

    void operator=(const VectorNative& other)
    {
        count = (int) other.size();
        if (allocated < count)
            reserve(count, false);
        memcpy(buffer, other.buffer, count * sizeof(T));
    }

    void reserve(int newcapacity, bool copy = true)
    {
        if (newcapacity <= allocated)
            return;

        allocated *= 2;
        if (newcapacity > allocated)
            allocated = newcapacity;
        allocated = max(allocated, 4);

        T* newPtr = (T*) g_Allocator.alloc(allocated * sizeof(T));
        SWAG_ASSERT(newPtr);

        if (copy)
            memcpy(newPtr, buffer, count * sizeof(T));

        buffer = newPtr;
    }

    void push_back(const T& val)
    {
        reserve(count + 1);
        buffer[count++] = val;
    }

    void push_front(const T& val)
    {
        reserve(count + 1);
        memmove(buffer + 1, buffer, count * sizeof(T));
        buffer[0] = val;
        count++;
    }

    void pop_back()
    {
        SWAG_ASSERT(count);
        count--;
    }

    void set_size_clear(int num)
    {
        reserve(num);
        memset(buffer, 0, num * sizeof(T));
        count = num;
    }

    T* begin()
    {
        return buffer;
    }

    T* end()
    {
        return buffer + count;
    }

    bool empty()
    {
        return count == 0;
    }

    void clear()
    {
        count = 0;
    }

    const T& back()
    {
        SWAG_ASSERT(count);
        return buffer[count - 1];
    }

    const T& front()
    {
        return buffer[0];
    }

    T& operator[](int index)
    {
        SWAG_ASSERT(buffer);
        return buffer[index];
    }

    void erase(int index)
    {
        if (index != count - 1)
            memmove(buffer + index, buffer + index + 1, (count - index - 1) * sizeof(T));
        count--;
    }

    void append(const VectorNative& other)
    {
        reserve(count + other.count);
        memcpy(buffer + count, other.buffer, other.count * sizeof(T));
        count += other.count;
    }
};
