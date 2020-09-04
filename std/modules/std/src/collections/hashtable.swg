namespace std.collections
{
    public struct(K, V) HashTableEntry
    {
        hash:   u32
        key:    K
        value:  V
    }

    public struct(K, V) HashTable
    {
        table:      *HashTableEntry'(K, V)
        count:      u32 // Number of valid entries
        allocated:  u32 // Number of total entries the table can hold
    }

    impl HashTable
    {
        public
        {
            const HASH_FREE = 0'u32
            const HASH_DELETED = 1'u32
            const HASH_FIRST = 2'u32

            func hash(using self, key: K)->u32
            {
                hashVal := 0

                if hashVal < HASH_FIRST
                    hashVal += HASH_FIRST
                return hashVal
            }

            func hashIndex(using self, hashVal: u32)->u32
            {
                return hashVal & (allocated - 1)
            }

            func add(using self, key: K, value: V)
            {
                hashVal := hash(key)
                hashIndex := hashIndex(hashVal)

                while table[hashIndex]
                {
                    hashIndex += 1
                    if hashIndex == allocated
                        hashIndex = 0
                }

                table[hashIndex].hash = hashVal
                table[hashIndex].key = key
                table[hashIndex].value = value
            }

            func contains(using self, key: K)->bool
            {
                hashVal := hash(key)
                hashIndex := hashIndex(hashVal)

                while table[hashIndex]
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
