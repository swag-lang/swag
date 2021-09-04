#global export
using Swag

struct(K) HashSetEntry
{
    hash:   u32
    key:    K
}

struct(K) HashSet
{
    const HASH_FREE = 0'u32
    const HASH_DELETED = 1'u32
    const HASH_FIRST = 2'u32

    allocator:  IAllocator
    table:      *HashSetEntry
    count:      uint // Number of valid entries
    capacity:   uint // Number of total entries the table can hold
    deleted:    uint // Number of deleted entries
}

impl HashSet
{
    mtd opDrop()
    {
        free()
    }

    #[Macro]
    mtdc(ptr: bool) opVisit(stmt: code)
    {
        if !count
            return

        remain := count
        scope Up
        loop capacity
        {
            if table[@index].hash < HASH_FIRST
                continue

            scope Down
            #macro
            {
                #if `ptr
                {
                    var @alias0 = &`table[@index].key
                }
                #else
                {
                    var @alias0 = Reflection.isStruct(K) ? cast(const *K) &`table[@index].key : `table[@index].key
                }

                #mixin `stmt { break = break Up; continue = break Down; }
            }

            remain -= 1
            if !remain
                break
        }
    }

    #[Inline]
    func hashKey(key: K)->u32
    {
        hashVal := Hash.hash32(key)
        if hashVal < HASH_FIRST
            hashVal += HASH_FIRST
        return hashVal
    }

    mtd free()
    {
        if !table
            return

        // Need to drop every structs
        #if Reflection.hasDrop(@typeof(K))
        {
            loop i: capacity
            {
                if table[i].hash >= HASH_FIRST
                {
                    @drop(&table[i].key)
                }
            }
        }

        Memory.free(table, capacity * @sizeof(HashSetEntry), allocator)
    }

    mtd grow()
    {
        // We need to trigger the growing of the table is there's not enough room
        if capacity != 0 and capacity - (count + deleted) > capacity / 4
            return

        newCapacity := capacity * 2
        reserve(newCapacity)
    }

    // Reserve 'newCapacity' elements in the table.
    mtd reserve(newCapacity: uint)
    {
        reqCapacity := Math.max(newCapacity, 32)
        if reqCapacity <= capacity
            return

        oldTable := table
        count = 0
        deleted = 0
        newCapacityBytes := reqCapacity * @sizeof(HashSetEntry)

        if allocator == null
            allocator = @getcontext().allocator
        table = acast Memory.alloc(newCapacityBytes, allocator)

        // Be sure hash entry is 0
        Memory.clear(table, newCapacityBytes)

        // Copy the last table to the new table, removing deleted entries
        loop i: capacity
        {
            if oldTable[i].hash >= HASH_FIRST
            {
                add(self, oldTable[i].key)
                @drop(&oldTable[i].key)
            }
        }

        capacity = reqCapacity
    }

    // Add a new key value pair. If the key already exists, then the value will be replaced
    #[Swag.Discardable]
    mtd add(key: K)->*HashSetEntry
        #selectif Reflection.canCopy(@typeof(K))
    {
        grow()

        hashVal := hashKey(key)
        index := hashVal % capacity

        while table[index].hash >= HASH_FIRST
        {
            if table[index].hash == hashVal and table[index].key == key
                return table + index
            index = (index + 1) % capacity
        }

        if table[index].hash == HASH_DELETED
            deleted -= 1
        table[index].hash = hashVal
        table[index].key = key
        count += 1
        return table + index
    }

    // Remove the given key if it exists
    mtd remove(key: K)
    {
        hashVal := hashKey(key)
        index := hashVal % capacity

        while table[index].hash
        {
            if table[index].hash == hashVal and table[index].key == key
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
    mtd contains(key: K)->bool
    {
        if !table
            return false

        hashVal := hashKey(key)
        index := hashVal % capacity

        while table[index].hash
        {
            if table[index].hash == hashVal and table[index].key == key
                return true
            index = (index + 1) % capacity
        }

        return false
    }

    // Remove all elements
    mtd clear()
    {
        // Need to put the 'hash' value of each entry to HASH_FREE
        // Drop the key and the value if necessary
        if count or deleted
        {
            loop capacity
            {
                hashval := table[@index].hash
                #if Reflection.hasDrop(@typeof(K))
                {
                    if hashval >= HASH_FIRST
                    {
                        @drop(&table[@index].key)
                    }
                }

                if hashval == HASH_DELETED
                    deleted -= 1
                else if hashval >= HASH_FIRST
                    count -= 1
                table[@index].hash = HASH_FREE

                // We are done if there's no more valid value, and no more deleted value
                if !count and !deleted
                    break
            }
        }
    }
}
