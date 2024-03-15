// ReSharper disable CppInconsistentNaming
#pragma once
#include "Core/Allocator.h"
#include "Report/Assert.h"

template<typename T>
struct VectorNative
{
    static_assert(std::is_trivially_destructible<T>());
    static_assert(std::is_trivially_copyable<T>());

    VectorNative() = default;

    VectorNative(const VectorNative& other)
    {
        allocated = 0;
        buffer    = nullptr;

        count = other.size();
        reserve(count, false);
        std::copy_n(other.buffer, count, buffer);
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

    VectorNative(const std::initializer_list<T>& other)
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
        Allocator::free_n_aligned(buffer, allocated);
        buffer    = nullptr;
        count     = 0;
        allocated = 0;
    }

    void reserve(uint32_t newCapacity, bool copy = true)
    {
        if (newCapacity <= allocated)
            return;

        const auto oldAllocated = allocated;
        allocated               = max(allocated * 2, 4);
        allocated               = max(allocated, newCapacity);
        auto newPtr             = Allocator::alloc_n_aligned<T>(allocated);
        if (copy && count)
            std::copy_n(buffer, count, newPtr);
        if (buffer)
            Allocator::free_n_aligned(buffer, oldAllocated);
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
        for (uint32_t i = 0; i < count / 2; i++)
            std::swap(buffer[i], buffer[count - i - 1]);
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

    T get_pop_back()
    {
        SWAG_ASSERT(count);
        count--;
        return buffer[count];
    }

    void expand_clear(size_t num)
    {
        if (num)
        {
            reserve(static_cast<uint32_t>(num), false);
            if (count < num)
                memset(buffer + count, 0, (num - count) * sizeof(T));
        }

        count = static_cast<uint32_t>(num);
    }

    void set_size_clear(size_t num)
    {
        if (num)
        {
            reserve(static_cast<uint32_t>(num), false);
            memset(buffer, 0, num * sizeof(T));
        }

        count = static_cast<uint32_t>(num);
    }

    void clear()
    {
        count = 0;
    }

    T* begin()
    {
        return buffer;
    }

    T* end()
    {
        return buffer + count;
    }

    const T* begin() const
    {
        return buffer;
    }

    const T* end() const
    {
        return buffer + count;
    }

    bool empty() const
    {
        return count == 0;
    }

    T& back()
    {
        SWAG_ASSERT(count);
        return buffer[count - 1];
    }

    const T& back() const
    {
        SWAG_ASSERT(count);
        return buffer[count - 1];
    }

    const T& front() const
    {
        return buffer[0];
    }

    void erase(size_t index, size_t num = 1)
    {
        if (index != count - num)
            memmove(buffer + index, buffer + index + num, (count - index - num) * sizeof(T));
        count -= static_cast<uint32_t>(num);
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
        std::copy_n(other.buffer, other.count, buffer + count);
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

    uint32_t capacity() const
    {
        return allocated;
    }

    uint32_t size() const
    {
        return count;
    }

    VectorNative& operator=(const Vector<T>& other)
    {
        count = other.size();
        if (allocated < count)
            reserve(count, false);
        std::copy_n(&other[0], count, buffer);
        return *this;
    }

    VectorNative& operator=(const VectorNative& other)
    {
        if (this != &other)
        {
            count = other.size();
            if (allocated < count)
                reserve(count, false);
            std::copy_n(other.buffer, count, buffer);
        }

        return *this;
    }

    VectorNative& operator=(VectorNative&& other) noexcept
    {
        count     = other.count;
        allocated = other.allocated;
        buffer    = other.buffer;

        other.count = other.allocated = 0;
        other.buffer                  = nullptr;
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
