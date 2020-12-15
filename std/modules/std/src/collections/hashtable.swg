#public
using swag

struct(K, V) HashTableEntry
{
    hash:   u32
    key:    K
    value:  V
}

struct(K, V) HashTable
{
    const HASH_FREE = 0'u32
    const HASH_DELETED = 1'u32
    const HASH_FIRST = 2'u32

    allocator:  IAllocator
    table:      *HashTableEntry
    count:      uint // Number of valid entries
    capacity:   uint // Number of total entries the table can hold
    deleted:    uint // Number of deleted entries
}

impl HashTable
{
    func opDrop(using self)
    {
        free()
    }

    #[macro]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        if !count return
        remain := count
        loop capacity
        {
            if table[@index].hash < HASH_FIRST
                continue
            #macro
            {
                #if `ptr
                {
                    var @alias0 = &`table[@index].key
                    var @alias1 = &`table[@index].value
                }
                #else
                {
                    var @alias0 = `table[@index].key
                    var @alias1 = `table[@index].value
                }

                #[swag.noreturn]
                #mixin `stmt
            }

            remain -= 1
            if !remain
                break
        }
    }

    #[inline]
    func hashKey(key: K)->u32
    {
        hashVal := hash.hash32(key)
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

        memory.free(table, capacity * @sizeof(HashTableEntry), allocator)
    }

    func grow(using self)
    {
        // We need to trigger the growing of the table is there's not enough room
        if capacity != 0 && capacity - (count + deleted) > capacity / 4
            return

        newCapacity := capacity * 2
        reserve(newCapacity)
    }

    // Reserve 'newCapacity' elements in the table.
    func reserve(using self, newCapacity: uint)
    {
        reqCapacity := math.max(newCapacity, 32)
        if(reqCapacity <= capacity)
            return

        oldTable := table
        count = 0
        deleted = 0
        newCapacityBytes := reqCapacity * @sizeof(HashTableEntry)

        if allocator == null
            allocator = @getcontext().allocator
        table = autocast memory.alloc(newCapacityBytes, allocator)

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
        grow()

        hashVal := hashKey(key)
        index := hashVal % capacity

        while table[index].hash >= HASH_FIRST
        {
            if table[index].hash == hashVal && table[index].key == key
            {
                table[index].value = value
                return
            }

            index = (index + 1) % capacity
        }

        if table[index].hash == HASH_DELETED
            deleted -= 1
        table[index].hash = hashVal
        table[index].key = key
        table[index].value = value
        count += 1
    }

    // Remove the given key if it exists
    func remove(using self, key: K)
    {
        hashVal := hashKey(key)
        index := hashVal % capacity

        while table[index].hash
        {
            if table[index].hash == hashVal && table[index].key == key
            {
                table[index].hash = HASH_DELETED
                count -= 1
                deleted += 1
                return
            }

            index = (index + 1) % capacity
        }
    }

    // Returns true if the table contains the given key
    func contains(using self, key: K)->bool
    {
        if !table return false

        hashVal := hashKey(key)
        index := hashVal % capacity

        while table[index].hash
        {
            if table[index].hash == hashVal && table[index].key == key
                return true
            index = (index + 1) % capacity
        }

        return false
    }

    // Find the given key, and returns the corresponding entry.
    // Returns null if the key is not there.
    func find(using self, key: K)->const *HashTableEntry
    {
        if !table return null

        hashVal := hashKey(key)
        index := hashVal % capacity

        while table[index].hash
        {
            if table[index].hash == hashVal && table[index].key == key
                return &table[index]
            index = (index + 1) % capacity
        }

        return null
    }

    // Remove all elements
    func clear(using self)
    {
        // Need to put the 'hash' value of each entry to HASH_FREE
        // Drop the key and the value if necessary
        if count || deleted
        {
            loop capacity
            {
                hashval := table[@index].hash
                #if reflection.hasDrop(@typeof(K)) || reflection.hasDrop(@typeof(V))
                {
                    if hashval >= HASH_FIRST
                    {
                        @drop(&table[@index].key)
                        @drop(&table[@index].value)
                    }
                }

                if hashval == HASH_DELETED
                    deleted -= 1
                else if hashval >= HASH_FIRST
                    count -= 1
                table[@index].hash = HASH_FREE

                // We are done if there's no more valid value, and no more deleted value
                if !count && !deleted
                    break
            }
        }
    }
}
