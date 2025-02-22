// ReSharper disable CppInconsistentNaming
#pragma once
#include "Core/Allocator.h"
#include "Report/Assert.h"

#ifdef SWAG_HAS_RACE_CONDITION_VECTOR
#define SWAG_RACE_CONDITION_WRITE_VECTOR(__x)    SWAG_RACE_CONDITION_ON_WRITE(__x)
#define SWAG_RACE_CONDITION_READ_VECTOR(__x)     SWAG_RACE_CONDITION_ON_READ(__x)
#define SWAG_RACE_CONDITION_INSTANCE_VECTOR(__x) SWAG_RACE_CONDITION_ON_INSTANCE(__x)
#else
#define SWAG_RACE_CONDITION_WRITE_VECTOR(__x)    SWAG_RACE_CONDITION_OFF_WRITE(__x)
#define SWAG_RACE_CONDITION_READ_VECTOR(__x)     SWAG_RACE_CONDITION_OFF_READ(__x)
#define SWAG_RACE_CONDITION_INSTANCE_VECTOR(__x) SWAG_RACE_CONDITION_OFF_INSTANCE(__x)
#endif

template<typename T>
struct VectorNative
{
    static_assert(std::is_trivially_destructible<T>());
    static_assert(std::is_trivially_copyable<T>());

    VectorNative() = default;

    VectorNative(const VectorNative& other)
    {
        SWAG_RACE_CONDITION_READ_VECTOR(other.raceC);
        allocated = 0;
        buffer    = nullptr;

        count = other.size();
        reserve(count, false);
        std::copy_n(other.buffer, count, buffer);
    }

    VectorNative(VectorNative&& other) noexcept
    {
        SWAG_RACE_CONDITION_READ_VECTOR(other.raceC);
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
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (!buffer)
            return;
        Allocator::freeAlignedN(buffer, allocated);
        buffer    = nullptr;
        count     = 0;
        allocated = 0;
    }

    void reserve(uint32_t newCapacity, bool copy = true)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (newCapacity <= allocated)
            return;

        const auto oldAllocated = allocated;
        allocated               = std::max(allocated * 2, static_cast<uint32_t>(4));
        allocated               = std::max(allocated, newCapacity);
        auto newPtr             = Allocator::allocAlignedN<T>(allocated);
        if (copy && count)
            std::copy_n(buffer, count, newPtr);
        if (buffer)
            Allocator::freeAlignedN(buffer, oldAllocated);
        buffer = newPtr;
    }

    T* reserve_back()
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (count + 1 > allocated)
            reserve(count + 1);
        count++;
        return buffer + count - 1;
    }

    void push_back(const T& val)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (count + 1 > allocated)
            reserve(count + 1);
        buffer[count++] = val;
    }

    void push_back_once(const T& val)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (contains(val))
            return;
        push_back(val);
    }

    void push_front(const T& val)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (count + 1 > allocated)
            reserve(count + 1);
        memmove(buffer + 1, buffer, count * sizeof(T));
        buffer[0] = val;
        count++;
    }

    void reverse()
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        for (uint32_t i = 0; i < count / 2; i++)
            std::swap(buffer[i], buffer[count - i - 1]);
    }

    void insert_at_index(const T& val, size_t index)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
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
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        SWAG_ASSERT(count);
        count--;
    }

    T get_pop_back()
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        SWAG_ASSERT(count);
        count--;
        return buffer[count];
    }

    void expand_clear(size_t num)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (num)
        {
            reserve(static_cast<uint32_t>(num), true);
            if (count < num)
                memset(buffer + count, 0, (num - count) * sizeof(T));
        }

        count = std::max(count, static_cast<uint32_t>(num));
    }

    void set_size_clear(size_t num)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (num)
        {
            reserve(static_cast<uint32_t>(num), false);
            memset(buffer, 0, num * sizeof(T));
        }

        count = static_cast<uint32_t>(num);
    }

    void clear()
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        count = 0;
    }

    T* begin()
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        return buffer;
    }

    T* end()
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        return buffer + count;
    }

    const T* begin() const
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        return buffer;
    }

    const T* end() const
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        return buffer + count;
    }

    bool empty() const
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        return count == 0;
    }

    T& back()
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        SWAG_ASSERT(count);
        return buffer[count - 1];
    }

    const T& back() const
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        SWAG_ASSERT(count);
        return buffer[count - 1];
    }

    const T& front() const
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        return buffer[0];
    }

    void erase(size_t index, size_t num = 1)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (index != count - num)
            memmove(buffer + index, buffer + index + num, (count - index - num) * sizeof(T));
        count -= static_cast<uint32_t>(num);
    }

    void erase_unordered(size_t index)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (index != count - 1)
            buffer[index] = buffer[count - 1];
        count--;
    }

    void erase_ordered_by_val(const T& val)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
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
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
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
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        reserve(count + other.count);
        std::copy_n(other.buffer, other.count, buffer + count);
        count += other.count;
    }

    int find(const T& value)
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
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
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        for (uint32_t i = 0; i < count; i++)
        {
            if (buffer[i] == val)
                return true;
        }

        return false;
    }

    uint32_t capacity() const
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        return allocated;
    }

    uint32_t size() const
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        return count;
    }

    VectorNative& operator=(const Vector<T>& other)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        count = other.size();
        if (allocated < count)
            reserve(count, false);
        std::copy_n(&other[0], count, buffer);
        return *this;
    }

    VectorNative& operator=(const VectorNative& other) // NOLINT(bugprone-unhandled-self-assignment)
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        if (this == &other)
            return *this;

        count = other.size();
        if (allocated < count)
            reserve(count, false);
        std::copy_n(other.buffer, count, buffer);

        return *this;
    }

    VectorNative& operator=(VectorNative&& other) noexcept
    {
        SWAG_RACE_CONDITION_WRITE_VECTOR(raceC);
        count     = other.count;
        allocated = other.allocated;
        buffer    = other.buffer;

        other.count = other.allocated = 0;
        other.buffer                  = nullptr;
        return *this;
    }

    const T& operator[](size_t index) const
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        return buffer[index];
    }

    T& operator[](size_t index)
    {
        SWAG_RACE_CONDITION_READ_VECTOR(raceC);
        SWAG_ASSERT(buffer && count);
        SWAG_ASSERT(index < count);
        return buffer[index];
    }

    T*       buffer    = nullptr;
    uint32_t count     = 0;
    uint32_t allocated = 0;

    SWAG_RACE_CONDITION_INSTANCE_VECTOR(raceC);
};
