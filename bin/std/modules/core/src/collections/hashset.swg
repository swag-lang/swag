#global export
using Swag

struct(K) HashSetEntry
{
    hash:   u32
    key:    K
}

struct(K) HashSet
{
    const HASH_FREE    = 0'u32
    const HASH_DELETED = 1'u32
    const HASH_FIRST   = 2'u32
    const HASH_MASK    = 2'u32

    allocator:  IAllocator
    table:      ^HashSetEntry
    count:      u64 // Number of valid entries
    capacity:   u64 // Number of total entries the table can hold
    deleted:    u64 // Number of deleted entries
}

impl HashSet
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

        let newTable = cast(^HashSetEntry) Memory.alloc(capacity * @sizeof(HashSetEntry), allocator)
        #if Reflection.hasPostCopy(K)
        {
            loop i: capacity
            {
                newTable[i].hash = table[i].hash
                newTable[i].key  =,nodrop table[i].key
            }
        }
        #else
        {
            Memory.copy(newTable, table, capacity * @sizeof(HashSetEntry))
        }

        table = newTable
    }

    #[Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: code)
    {
        if !count
            return

        var remain = count
        #scope Up
        loop capacity
        {
            if table[@index].hash < HASH_FIRST
                continue

            #scope Down
            #macro
            {
                #if #up ptr
                {
                    let @alias0 = &#up table[@index].key
                }
                #elif Reflection.isStruct(K)
                {
                    let @alias0 = cast(const &K) &#up table[@index].key
                }
                #else
                {
                    let @alias0 = #up table[@index].key
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

    mtd free()
    {
        if !table
            return

        // Need to drop every structs
        #if Reflection.hasDrop(@typeof(K))
        {
            loop i: capacity
            {
                if table[i].hash >= HASH_DELETED
                    @drop(&table[i].key)
            }
        }

        Memory.free(table, capacity * @sizeof(HashSetEntry), allocator)
    }

    mtd grow()
    {
        // We need to trigger the growing of the table is there's not enough room
        if capacity != 0 and capacity - (count + deleted) > capacity / 4
            return

        let newCapacity = capacity * 2
        reserve(newCapacity)
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
        let newCapacityBytes = reqCapacity * @sizeof(HashSetEntry)

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
                emplaceInternal(h, &oldTable[i].key)
            }
            elif h == HASH_DELETED
            {
                @drop(&oldTable[i].key)
            }
        }

        Memory.free(oldTable, oldCapacity * @sizeof(HashSetEntry), allocator)
    }

    // Add a new key.
    #[Discardable, Overload]
    mtd add(key: K)->*HashSetEntry
        #validif Reflection.canCopy(@typeof(K))
    {
        grow()

        let hashVal = hashKey(key)
        var index = hashVal % capacity

        while table[index].hash >= HASH_FIRST
        {
            if table[index].hash == hashVal and table[index].key == key
                return table + index
            index = (index + 1) % capacity
        }

        if table[index].hash == HASH_FREE
        {
            table[index].key =,nodrop key
        }
        else
        {
            if table[index].hash == HASH_DELETED
                deleted -= 1
            table[index].key = key
        }

        table[index].hash = hashVal
        count += 1
        return table + index
    }

    // Add a new key.
    #[Discardable, Overload]
    mtd add(key: &&K)->*HashSetEntry
    {
        grow()

        let hashVal = hashKey(key)
        var index = hashVal % capacity

        while table[index].hash >= HASH_FIRST
        {
            if table[index].hash == hashVal and table[index].key == key
                return table + index
            index = (index + 1) % capacity
        }

        if table[index].hash == HASH_FREE
        {
            table[index].key =,nodrop,move key
        }
        else
        {
            if table[index].hash == HASH_DELETED
                deleted -= 1
            table[index].key =,move key
        }

        table[index].hash = hashVal
        count += 1
        return table + index
    }

    // Add a new key.
    #[Discardable]
    mtd emplaceInternal(hashVal: u32, key: &K)->*HashSetEntry
        #validif Reflection.canCopy(@typeof(K))
    {
        grow()

        var index = hashVal % capacity

        while table[index].hash >= HASH_FIRST
        {
            if table[index].hash == hashVal and table[index].key == key
                return table + index
            index = (index + 1) % capacity
        }

        if table[index].hash == HASH_FREE
        {
            table[index].key =,nodrop,moveraw key
        }
        else
        {
            if table[index].hash == HASH_DELETED
                deleted -= 1
            table[index].key =,moveraw key
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
                    @drop(&table[i].key)

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
