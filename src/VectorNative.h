#pragma once
#include "Allocator.h"
#include "Assert.h"

template<typename T>
struct VectorNative
{
    VectorNative() = default;

    VectorNative(const VectorNative& other)
    {
        allocated = 0;
        buffer    = nullptr;

        count = (int) other.size();
        reserve(count, false);
        memcpy(buffer, other.buffer, count * sizeof(T));
    }

    ~VectorNative()
    {
        if (buffer)
            g_Allocator.free(buffer, Allocator::alignSize(allocated * sizeof(T)));
    }

    void reserve(int newcapacity, bool copy = true)
    {
        if (newcapacity <= allocated)
            return;

        auto oldAllocated = allocated;
        allocated *= 2;
        allocated = max(allocated, 4);
        allocated = max(allocated, newcapacity);
        T* newPtr = (T*) g_Allocator.alloc(Allocator::alignSize(allocated * sizeof(T)));
        SWAG_ASSERT(newPtr);
        if (copy && count)
            memcpy(newPtr, buffer, count * sizeof(T));
        if (buffer)
            g_Allocator.free(buffer, Allocator::alignSize(oldAllocated * sizeof(T)));
        buffer = newPtr;
    }

    T* reserve_back()
    {
        if (count + 1 > allocated)
            reserve(count + 1);
        count++;
        return buffer + count - 1;
    }

    void push_back(const T& val)
    {
        if (count + 1 > allocated)
            reserve(count + 1);
        buffer[count++] = val;
    }

    void push_back_once(const T& val)
    {
        if (contains(val))
            return;
        push_back(val);
    }

    void push_front(const T& val)
    {
        if (count + 1 > allocated)
            reserve(count + 1);
        memmove(buffer + 1, buffer, count * sizeof(T));
        buffer[0] = val;
        count++;
    }

    void insertAtIndex(const T& val, int index)
    {
        if (index == count)
        {
            push_back(val);
            return;
        }

        SWAG_ASSERT(index < count);
        if (count + 1 > allocated)
            reserve(count + 1);
        memmove(buffer + index + 1, buffer + index, (count - index) * sizeof(T));
        buffer[index] = val;
        count++;
    }

    void pop_back()
    {
        SWAG_ASSERT(count);
        count--;
    }

    void set_size_clear(int num)
    {
        if (num)
        {
            reserve(num, false);
            memset(buffer, 0, num * sizeof(T));
        }

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

    const T& back() const
    {
        SWAG_ASSERT(count);
        return buffer[count - 1];
    }

    T& back()
    {
        SWAG_ASSERT(count);
        return buffer[count - 1];
    }

    const T& front()
    {
        return buffer[0];
    }

    void erase(int index, int num = 1)
    {
        if (index != count - num)
            memmove(buffer + index, buffer + index + num, (count - index - num) * sizeof(T));
        count -= num;
    }

    void erase_unordered(int index)
    {
        if (index != count - 1)
            buffer[index] = buffer[count - 1];
        count--;
    }

    void erase_unordered_byval(const T& val)
    {
        for (int i = 0; i < count; i++)
        {
            if (buffer[i] == val)
            {
                erase_unordered(i);
                return;
            }
        }

        SWAG_ASSERT(false);
    }

    void append(const VectorNative& other)
    {
        if (!other.count)
            return;
        reserve(count + other.count);
        memcpy(buffer + count, other.buffer, other.count * sizeof(T));
        count += other.count;
    }

    bool contains(const T& val)
    {
        for (int i = 0; i < count; i++)
        {
            if (buffer[i] == val)
                return true;
        }

        return false;
    }

    void insert(const T& val)
    {
        if (!contains(val))
            push_back(val);
    }

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

    void operator=(VectorNative&& other)
    {
        count     = other.count;
        allocated = other.allocated;
        buffer    = other.buffer;

        other.count = other.allocated = 0;
        other.buffer                  = nullptr;
    }

    const T& operator[](int index) const
    {
        return buffer[index];
    }

    T& operator[](int index)
    {
        SWAG_ASSERT(buffer && count);
        SWAG_ASSERT(index < count);
        return buffer[index];
    }

    T*  buffer    = nullptr;
    int count     = 0;
    int allocated = 0;
};
