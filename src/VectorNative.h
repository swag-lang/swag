// ReSharper disable CppInconsistentNaming
#pragma once
#include "Allocator.h"
#include "Assert.h"

template<typename T>
struct VectorNative
{
    static_assert(is_trivially_destructible<T>());
    static_assert(is_trivially_copyable<T>());

    VectorNative() = default;

    VectorNative(const VectorNative& other)
    {
        allocated = 0;
        buffer    = nullptr;

        count = (int) other.size();
        reserve(count, false);
        memcpy(buffer, other.buffer, count * sizeof(T));
    }

    VectorNative(VectorNative&& other) noexcept
    {
        buffer          = other.buffer;
        count           = other.count;
        allocated       = other.allocated;
        other.buffer    = nullptr;
        other.count     = 0;
        other.allocated = 0;
    }

    VectorNative(const initializer_list<T>& other)
    {
        for (auto it : other)
            push_back(it);
    }

    ~VectorNative()
    {
        release();
    }

    void release()
    {
        if (!buffer)
            return;
        Allocator::free(buffer, Allocator::alignSize(allocated * sizeof(T)));
        buffer    = nullptr;
        count     = 0;
        allocated = 0;
    }

    void reserve(uint32_t newCapacity, bool copy = true)
    {
        if (newCapacity <= allocated)
            return;

        const auto oldAllocated = allocated;
        allocated *= 2;
        allocated = max(allocated, 4);
        allocated = max(allocated, newCapacity);
        T* newPtr = (T*) Allocator::alloc(Allocator::alignSize(allocated * sizeof(T)));
        SWAG_ASSERT(newPtr);
        if (copy && count)
            memcpy(newPtr, buffer, count * sizeof(T));
        if (buffer)
            Allocator::free(buffer, Allocator::alignSize(oldAllocated * sizeof(T)));
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

    void reverse()
    {
        for (size_t i = 0; i < count / 2; i++)
            swap(buffer[i], buffer[count - i - 1]);
    }

    void insert_at_index(const T& val, size_t index)
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

    [[nodiscard]] T get_pop_back()
    {
        SWAG_ASSERT(count);
        count--;
        return buffer[count];
    }

    void expand_clear(size_t num)
    {
        if (num)
        {
            reserve((uint32_t) num, false);
            if (count < num)
                memset(buffer + count, 0, (num - count) * sizeof(T));
        }

        count = (uint32_t) num;
    }

    void set_size_clear(size_t num)
    {
        if (num)
        {
            reserve((uint32_t) num, false);
            memset(buffer, 0, num * sizeof(T));
        }

        count = (uint32_t) num;
    }

    void clear()
    {
        count = 0;
    }    

    [[nodiscard]] T* begin()
    {
        return buffer;
    }

    [[nodiscard]] T* end()
    {
        return buffer + count;
    }

    [[nodiscard]] const T* begin() const
    {
        return buffer;
    }

    [[nodiscard]] const T* end() const
    {
        return buffer + count;
    }

    [[nodiscard]] bool empty() const
    {
        return count == 0;
    }

    [[nodiscard]] T& back()
    {
        SWAG_ASSERT(count);
        return buffer[count - 1];
    }

    [[nodiscard]] const T& back() const
    {
        SWAG_ASSERT(count);
        return buffer[count - 1];
    }    

    [[nodiscard]] const T& front() const
    {
        return buffer[0];
    }

    void erase(size_t index, size_t num = 1)
    {
        if (index != count - num)
            memmove(buffer + index, buffer + index + num, (count - index - num) * sizeof(T));
        count -= (uint32_t) num;
    }

    void erase_unordered(size_t index)
    {
        if (index != count - 1)
            buffer[index] = buffer[count - 1];
        count--;
    }

    void erase_ordered_by_val(const T& val)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            if (buffer[i] == val)
            {
                erase(i);
                return;
            }
        }
    }

    void erase_unordered_by_val(const T& val)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            if (buffer[i] == val)
            {
                erase_unordered(i);
                return;
            }
        }
    }

    void append(const VectorNative& other)
    {
        if (!other.count)
            return;
        reserve(count + other.count);
        memcpy(buffer + count, other.buffer, other.count * sizeof(T));
        count += other.count;
    }

    int find(const T& value)
    {
        int idx = 0;
        for (auto& it : *this)
        {
            if (it == value)
                return idx;
            idx++;
        }

        return -1;
    }

    bool contains(const T& val)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            if (buffer[i] == val)
                return true;
        }

        return false;
    }

    [[nodiscard]] size_t capacity() const
    {
        return allocated;
    }

    [[nodiscard]] size_t size() const
    {
        return count;
    }

    VectorNative& operator=(const Vector<T>& other)
    {
        count = (int) other.size();
        if (allocated < count)
            reserve(count, false);
        memcpy(buffer, &other[0], count * sizeof(T));
        return *this;
    }

    VectorNative& operator=(const VectorNative& other)
    {
        if(&other == this)
            return *this;
        
        count = (int) other.size();
        if (allocated < count)
            reserve(count, false);
        memcpy(buffer, other.buffer, count * sizeof(T));
        return *this;
    }

    VectorNative& operator=(VectorNative&& other) noexcept
    {
        count     = other.count;
        allocated = other.allocated;
        buffer    = other.buffer;

        other.count  = other.allocated = 0;
        other.buffer = nullptr;
        return *this;
    }

    const T& operator[](size_t index) const
    {
        return buffer[index];
    }

    T& operator[](size_t index)
    {
        SWAG_ASSERT(buffer && count);
        SWAG_ASSERT(index < count);
        return buffer[index];
    }

    T*       buffer    = nullptr;
    uint32_t count     = 0;
    uint32_t allocated = 0;
};
