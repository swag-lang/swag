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

            func opDrop(self)
            {
                self.free()
            }

            #[inline]
            func hash(key: K)->u32
            {
                hashVal := std.crypto.hash32(key)
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
                    loop i: allocated
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
                if allocated != 0 && allocated - (count + removed) > allocated / 4
                    return

                if allocator == null
                    allocator = @getcontext().allocator

                newAllocated := allocated * 2
                if(newAllocated < 32) newAllocated = 32

                oldTable := table
                count = 0
                removed = 0
                newAllocatedBytes := newAllocated * @sizeof(HashTableEntry)
                table = acast memory.alloc(&allocator, newAllocatedBytes)

                // Be sure hash entry is 0
                memory.clear(table, newAllocatedBytes)

                @print("newAllocatedBytes=")
                @print(cast(s64)@sizeof(HashTableEntry))
                @print("\n")
                loop i: newAllocated
                {
                    @print("v")
                    @print(cast(s64) i)
                    @print("=")
                    @print(cast(s64) table[i].hash)
                    @print(" ")
                }

                // Copy the last table to the new table, removing deleted entries
                loop i: allocated
                {
                    if oldTable[i].hash >= HASH_FIRST
                    {
                        add(self, oldTable[i].key, oldTable[i].value)
                        @drop(&oldTable[i].key)
                        @drop(&oldTable[i].value)
                    }
                }

                allocated = newAllocated
            }

            // Add a new key value pair. If the key already exists, then the value will be replaced
            func add(using self, key: K, value: V)
            {
                grow(self)

                hashVal := hash(key)
                index := hashVal % allocated

                while table[index].hash
                {
                    if table[index].hash == hashVal && table[index].key == key
                    {
                        table[index].value = value
                        return
                    }

                    index += 1
                    if index == allocated
                        index = 0
                }

                table[index].hash = hashVal
                table[index].key = key
                table[index].value = value
                count += 1
            }

            // Returns true if the table contains the given key
            func contains(using self, key: K)->bool
            {
                if !table return false

                hashVal := hash(key)
                index := hashVal % allocated

                while table[index].hash
                {
                    if table[index].hash == hashVal && table[index].key == key
                        return true
                    index += 1
                    if index == allocated
                        index = 0
                }

                return false
            }
        }
    }
}
