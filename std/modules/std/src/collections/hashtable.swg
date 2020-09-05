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
        table:      *HashTableEntry'(K, V)
        count:      u32 // Number of valid entries
        allocated:  u32 // Number of total entries the table can hold
        removed:    u32 // Number of removed entries
    }

    impl HashTable
    {
        public
        {
            const HASH_FREE = 0'u32
            const HASH_DELETED = 1'u32
            const HASH_FIRST = 2'u32

            func hash(key: K)->u32
            {
                hashVal := 0'u32

                if hashVal < HASH_FIRST
                    hashVal += HASH_FIRST
                return hashVal
            }

            func hashIndex(using self, hashVal: u32)->u32
            {
                return hashVal & (allocated - 1)
            }

            func free(using self)
            {
                if !table
                    return

                // Need to drop every structs
                #if reflection.hasMember(@typeof(K), "opDrop") || reflection.hasMember(@typeof(V), "opDrop")
                {
                    loop i: allocated
                    {
                        if table[i].hash >= HASH_FIRST
                        {
                            @drop(table[i].key)
                            @drop(table[i].value)
                        }
                    }
                }

                memory.free(&allocator, table)
            }

            func grow(using self)
            {
                if allocator == null
                    allocator = @getcontext().allocator

                newCapacity := allocated * 2
                if(newCapacity < 32) newCapacity = 32

                oldTable := table
                count = 0
                removed = 0
                table := acast memory.alloc(&allocator, newCapacity * @sizeof(HashTableEntry'(K, V)))

                // Copy the last table to the next table, removing deleted entries
                loop i: allocated
                {
                    if oldTable[i].hash >= HASH_FIRST
                        add(self, oldTable[i].key, oldTable[i].value)
                }

                allocated = newCapacity
            }

            // Add a new key value pair. If the key already exists, then the value will be replaced
            func add(using self, key: K, value: V)
            {
                hashVal := hash(key)

                if !allocated
                    grow(self)

                hashIndex := hashIndex(self, hashVal)
                while table[hashIndex].hash
                {
                    if table[hashIndex].hash == hashVal && table[hashIndex].key == key
                    {
                        table[hashIndex].value = value
                        return
                    }

                    hashIndex += 1
                    if hashIndex == allocated
                        hashIndex = 0
                }

                table[hashIndex].hash = hashVal
                table[hashIndex].key = key
                table[hashIndex].value = value
                count += 1
            }

            // Returns true if the table contains the given key
            func contains(using self, key: K)->bool
            {
                if !table return false
                hashVal := hash(key)
                hashIndex := hashIndex(self, hashVal)

                while table[hashIndex].hash
                {
                    if table[hashIndex].hash == hashVal && table[hashIndex].key == key
                        return true
                    hashIndex += 1
                    if hashIndex == allocated
                        hashIndex = 0
                }

                return false
            }
        }
    }
}
