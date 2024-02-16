#pragma once

template<typename T>
struct Vector : vector<T, StdAllocator<T>>
{
	Vector()
		: vector<T, StdAllocator<T>>()
	{
	}

	Vector(const initializer_list<T>& other)
		: vector<T, StdAllocator<T>>(other)
	{
	}

	bool contains(const T& value)
	{
		for (auto& it : *this)
		{
			if (it == value)
				return true;
		}

		return false;
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

	void release() noexcept
	{
		this->~Vector<T>();
		::new(this) Vector;
	}
};

template<typename K, typename V>
struct VectorMap : Vector<pair<K, V>>
{
	using It = typename Vector<pair<K, V>>::iterator;

	It find(const K& key)
	{
		for (auto it = this->begin(); it != this->end(); ++it)
		{
			if (it->first == key)
				return it;
		}

		return this->end();
	}

	V& operator[](const K& key)
	{
		auto it = find(key);
		if (it != this->end())
			return it->second;
		pair<K, V> tmp;
		tmp.first = key;
		this->emplace_back(std::move(tmp));
		return this->back().second;
	}
};
