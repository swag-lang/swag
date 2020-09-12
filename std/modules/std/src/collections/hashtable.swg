namespace std.collections
{
    using swag

    public struct(K, V) HashTableEntry
    {
        hash:   u32
        key:    K
        value:  V
    }

    public struct(K, V) HashTable
    {
        allocator:  const IAllocator
        table:      *HashTableEntry
        count:      u32 // Number of valid entries
        capacity:   u32 // Number of total entries the table can hold
        removed:    u32 // Number of removed entries
    }

    public impl HashTable
    {
        const HASH_FREE = 0'u32
        const HASH_DELETED = 1'u32
        const HASH_FIRST = 2'u32

        func opDrop(self)
        {
            self.free()
        }

        #[macro, noreturn]
        func(ptr: bool) opVisit(using self, stmt: code)
        {
            if count
            {
                remain := count
                loop capacity
                {
                    if table[@index].hash < HASH_FIRST
                        continue
                    #macro
                    {
                        #if ptr
                        {
                            let @alias0 = &table[@index].key
                            let @alias1 = &table[@index].value
                        }
                        #else
                        {
                            let @alias0 = table[@index].key
                            let @alias1 = table[@index].value
                        }

                        #mixin `stmt
                    }

                    remain -= 1
                    if !remain
                        break
                }
            }
        }

        #[inline]
        func hash(key: K)->u32
        {
            hashVal := crypto.hash32(key)
            if hashVal < HASH_FIRST
                hashVal += HASH_FIRST
            return hashVal
        }

        func free(using self)
        {
            if !table
                return

            // Need to drop every structs
            #if reflection.hasDrop(@typeof(K)) || reflection.hasDrop(@typeof(V))
            {
                loop i: capacity
                {
                    if table[i].hash >= HASH_FIRST
                    {
                        @drop(&table[i].key)
                        @drop(&table[i].value)
                    }
                }
            }

            memory.free(&allocator, table)
        }

        func grow(using self)
        {
            // We need to trigger the growing of the table is there's not enough room
            if capacity != 0 && capacity - (count + removed) > capacity / 4
                return

            newCapacity := capacity * 2
            reserve(newCapacity)
        }

        // Reserve 'newCapacity' elements in the table.
        func reserve(using self, newCapacity: u32)
        {
            reqCapacity := math.max(newCapacity, 32)
            if(reqCapacity <= capacity)
                return

            oldTable := table
            count = 0
            removed = 0
            newCapacityBytes := reqCapacity * @sizeof(HashTableEntry)

            if allocator == null
                allocator = @getcontext().allocator
            table = acast memory.alloc(&allocator, newCapacityBytes)

            // Be sure hash entry is 0
            memory.clear(table, newCapacityBytes)

            // Copy the last table to the new table, removing deleted entries
            loop i: capacity
            {
                if oldTable[i].hash >= HASH_FIRST
                {
                    add(self, oldTable[i].key, oldTable[i].value)
                    @drop(&oldTable[i].key)
                    @drop(&oldTable[i].value)
                }
            }

            capacity = reqCapacity
        }

        // Add a new key value pair. If the key already exists, then the value will be replaced
        func add(using self, key: K, value: V)
        {
            grow(self)

            hashVal := hash(key)
            index := hashVal % capacity

            while table[index].hash >= HASH_FIRST
            {
                if table[index].hash == hashVal && table[index].key == key
                {
                    table[index].value = value
                    return
                }

                index += 1
                if index == capacity
                    index = 0
            }

            if table[index].hash == HASH_DELETED
                removed -= 1
            table[index].hash = hashVal
            table[index].key = key
            table[index].value = value
            count += 1
        }

        // Remove the given key if it exists
        func remove(using self, key: K)
        {
            hashVal := hash(key)
            index := hashVal % capacity

            while table[index].hash
            {
                if table[index].hash == hashVal && table[index].key == key
                {
                    table[index].hash = HASH_DELETED;
                    count -= 1
                    removed += 1
                    return
                }

                index += 1
                if index == capacity
                    index = 0
            }
        }

        // Returns true if the table contains the given key
        func contains(using self, key: K)->bool
        {
            if !table return false

            hashVal := hash(key)
            index := hashVal % capacity

            while table[index].hash
            {
                if table[index].hash == hashVal && table[index].key == key
                    return true
                index += 1
                if index == capacity
                    index = 0
            }

            return false
        }
    }
}
