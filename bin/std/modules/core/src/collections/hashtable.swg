#global export
using Swag

struct(K, V) HashTableEntry
{
    hash:   u32
    key:    K
    value:  V
}

struct(K, V) HashTable
{
    const HASH_FREE    = 0'u32
    const HASH_DELETED = 1'u32
    const HASH_FIRST   = 2'u32
    const HASH_MASK    = 2'u32

    allocator:  IAllocator
    table:      ^HashTableEntry
    count:      u64 // Number of valid entries
    capacity:   u64 // Number of total entries the table can hold
    deleted:    u64 // Number of deleted entries
}

impl HashTable
{
    #[Inline]
    mtd opCount()->u64
    {
        return count
    }

    mtd opDrop()
    {
        free()
    }

    mtd opPostCopy()
    {
        if !table
            return

        let newTable = cast(^HashTableEntry) Memory.alloc(capacity * @sizeof(HashTableEntry), allocator)
        #if Reflection.hasPostCopy(K) or Reflection.hasPostCopy(V)
        {
            loop i: capacity
            {
                newTable[i].hash  = table[i].hash
                newTable[i].key   =,nodrop table[i].key
                newTable[i].value =,nodrop table[i].value
            }
        }
        #else
        {
            Memory.copy(newTable, table, capacity * @sizeof(HashTableEntry))
        }

        table = newTable
    }

    // alias0 is the key, and alias1 is the value
    #[Macro]
    mtd(ptr: bool) const opVisit(stmt: code)
    {
        if !count
            return

        var remain = count
        #scope Up loop capacity
        {
            if table[@index].hash < HASH_FIRST
                continue

            #scope Down
            #macro
            {
                #if #up ptr
                {
                    var @alias0 = &#up table[@index].key
                    var @alias1 = &#up table[@index].value
                }
                #else
                {
                    #if Reflection.isStruct(K)
                        var @alias0 = cast(const ref K) &#up table[@index].key
                    #else
                        var @alias0 = #up table[@index].key
                    #if Reflection.isStruct(V)
                        var @alias1 = cast(const ref V) &#up table[@index].value
                    #else
                        var @alias1 = #up table[@index].value
                }

                #mixin #up stmt { break = break Up; continue = break Down; }
            }

            remain -= 1
            if !remain
                break
        }
    }

    #[Inline]
    func hashKey(key: K)->u32
    {
        return Hash.hash32(key) | HASH_MASK
    }

    mtd grow()
    {
        // We need to trigger the growing of the table is there's not enough room
        if capacity != 0 and capacity - (count + deleted) > capacity / 4
            return

        let newCapacity = capacity * 2
        reserve(newCapacity)
    }

    // Free the hashtable content
    mtd free()
    {
        if !table
            return

        // Need to drop every structs
        #if Reflection.hasDrop(@typeof(K)) or Reflection.hasDrop(@typeof(V))
        {
            loop i: capacity
            {
                if table[i].hash >= HASH_DELETED
                {
                    @drop(&table[i].key)
                    @drop(&table[i].value)
                }
            }
        }

        Memory.free(table, capacity * @sizeof(HashTableEntry), allocator)
        table = null
        count, capacity, deleted = 0
    }

    // Reserve 'newCapacity' elements in the table.
    mtd reserve(newCapacity: u64)
    {
        let reqCapacity = Math.max(newCapacity, 32)
        if reqCapacity <= capacity
            return

        let oldTable = table
        count = 0
        deleted = 0
        let newCapacityBytes = reqCapacity * @sizeof(HashTableEntry)

        if allocator == null
            allocator = @getcontext().allocator
        table = acast Memory.alloc(newCapacityBytes, allocator)

        // Be sure hash entry is 0
        Memory.clear(table, newCapacityBytes)

        // Copy the last table to the new table, removing deleted entries
        let oldCapacity = capacity
        capacity = reqCapacity
        loop i: oldCapacity
        {
            let h = oldTable[i].hash
            if h >= HASH_FIRST
            {
                emplaceInternal(h, &oldTable[i].key, &oldTable[i].value)
            }
            elif h == HASH_DELETED
            {
                @drop(&oldTable[i].key)
                @drop(&oldTable[i].value)
            }
        }

        Memory.free(oldTable, oldCapacity * @sizeof(HashTableEntry), allocator)
    }

    // Add a new key value pair. If the key already exists, then the value will be replaced
    #[Discardable]
    mtd add(key: K, value: V)->*HashTableEntry
        #validif Reflection.canCopy(@typeof(K)) and Reflection.canCopy(@typeof(V))
    {
        grow()

        let hashVal = hashKey(key)
        var index = hashVal % capacity

        while table[index].hash >= HASH_FIRST
        {
            if table[index].hash == hashVal and table[index].key == key
            {
                table[index].value = value
                return table + index
            }

            index = (index + 1) % capacity
        }

        if table[index].hash == HASH_FREE
        {
            table[index].key =,nodrop key
            table[index].value =,nodrop value
        }
        else
        {
            if table[index].hash == HASH_DELETED
                deleted -= 1
            table[index].key = key
            table[index].value = value
        }

        table[index].hash = hashVal
        count += 1
        return table + index
    }

    // Add a new key value pair. If the key already exists, then the value will be replaced
    #[Discardable]
    mtd add(key: K, value: moveref V)->*HashTableEntry
        #validif Reflection.canCopy(@typeof(K))
    {
        grow()

        let hashVal = hashKey(key)
        var index = hashVal % capacity

        while table[index].hash >= HASH_FIRST
        {
            if table[index].hash == hashVal and table[index].key == key
            {
                table[index].value =,move value
                return table + index
            }

            index = (index + 1) % capacity
        }

        if table[index].hash == HASH_FREE
        {
            table[index].key =,nodrop key
            table[index].value =,nodrop,move value
        }
        else
        {
            if table[index].hash == HASH_DELETED
                deleted -= 1
            table[index].key = key
            table[index].value =,move value
        }

        table[index].hash = hashVal
        count += 1
        return table + index
    }

    // Use with care !
    #[Discardable]
    mtd emplaceInternal(hashVal: u32, key: ref K, value: ref V)->*HashTableEntry
        #validif Reflection.canCopy(@typeof(K)) and Reflection.canCopy(@typeof(V))
    {
        grow()

        var index = hashVal % capacity

        while table[index].hash >= HASH_FIRST
        {
            if table[index].hash == hashVal and table[index].key == key
            {
                table[index].value =,moveraw value
                return table + index
            }

            index = (index + 1) % capacity
        }

        if table[index].hash == HASH_FREE
        {
            table[index].key =,nodrop,moveraw key
            table[index].value =,nodrop,moveraw value
        }
        else
        {
            if table[index].hash == HASH_DELETED
                deleted -= 1
            table[index].key =,moveraw key
            table[index].value =,moveraw value
        }

        table[index].hash = hashVal
        count += 1
        return table + index
    }

    // Remove the given key if it exists
    mtd remove(key: K)
    {
        if !table
            return

        let hashVal = hashKey(key)
        var index = hashVal % capacity

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

        let hashVal = hashKey(key)
        var index = hashVal % capacity

        while table[index].hash
        {
            if table[index].hash == hashVal and table[index].key == key
                return true
            index = (index + 1) % capacity
        }

        return false
    }

    // Find the given key, and returns the corresponding entry.
    // Returns null if the key is not there.
    mtd find(key: K)->*HashTableEntry
    {
        if !table
            return null

        let hashVal = hashKey(key)
        var index = hashVal % capacity

        while table[index].hash
        {
            if table[index].hash == hashVal and table[index].key == key
                return table + index
            index = (index + 1) % capacity
        }

        return null
    }

    // Remove all elements
    mtd clear()
    {
        // Need to put the 'hash' value of each entry to HASH_FREE
        // Drop the key and the value if necessary
        if count or deleted
        {
            loop i: capacity
            {
                let hashval = table[i].hash

                if hashval >= HASH_DELETED
                {
                    @drop(&table[i].key)
                    @drop(&table[i].value)
                }

                if hashval == HASH_DELETED
                    deleted -= 1
                elif hashval >= HASH_FIRST
                    count -= 1
                table[i].hash = HASH_FREE

                // We are done if there's no more valid value, and no more deleted value
                if !count and !deleted
                    break
            }
        }
    }
}
