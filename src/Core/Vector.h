#pragma once

template<typename T>
struct Vector : std::vector<T, StdAllocator<T>>
{
    Vector() :
        std::vector<T, StdAllocator<T>>()
    {
    }

    Vector(const std::initializer_list<T>& other) :
        std::vector<T, StdAllocator<T>>(other)
    {
    }

    uint32_t size() const
    {
        return static_cast<uint32_t>(std::vector<T, StdAllocator<T>>::size());
    }

    bool contains(const T& value) const
    {
        for (auto& it : *this)
        {
            if (it == value)
                return true;
        }

        return false;
    }

    int find(const T& value) const
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

    void release() noexcept
    {
        this->~Vector<T>();
        ::new(this) Vector;
    }

    void append(const Vector& other)
    {
        this->insert(this->end(), other.begin(), other.end());
    }
};

template<typename K, typename V>
struct VectorMap : Vector<std::pair<K, V>>
{
    using It      = typename Vector<std::pair<K, V>>::iterator;
    using ConstIt = typename Vector<std::pair<K, V>>::const_iterator;

    ConstIt find(const K& key) const
    {
        for (auto it = this->begin(); it != this->end(); ++it)
        {
            if (it->first == key)
                return it;
        }

        return this->end();
    }

    It find(const K& key)
    {
        for (auto it = this->begin(); it != this->end(); ++it)
        {
            if (it->first == key)
                return it;
        }

        return this->end();
    }

    bool contains(const K& key) const
    {
        return find(key) != this->end();
    }

    V& operator[](const K& key)
    {
        auto it = find(key);
        if (it != this->end())
            return it->second;
        std::pair<K, V> tmp;
        tmp.first = key;
        this->emplace_back(std::move(tmp));
        return this->back().second;
    }
};
