#global export
using Swag

// Represents a single key-value pair entry within a HashTable.
struct(K, V) HashTableEntry
{
    hash:      u32     // The cached hash of the key.
    key:       K       // The key.
    value:     V       // The value associated with the key.
}

// A hash table implemented with open-addressing that maps keys to values.
// It provides fast lookups, additions, and removals of key-value pairs.
struct(K, V) HashTable
{
    // Special hash values to indicate the state of a slot in the table.
    const HASH_FREE        = 0'u32             // The slot is empty and has never been used.
    const HASH_DELETED     = 1'u32             // The slot was previously occupied but has been deleted.
    const HASH_FIRST       = 2'u32             // The first possible valid hash value.
    const HASH_MASK        = 2'u32             // A mask to ensure that valid hashes are >= HASH_FIRST.

    allocator:             IAllocator          // The allocator used for memory operations.
    table:                 ^HashTableEntry     // Pointer to the contiguous memory block for the hash table.
    count:                 u64                 // The number of valid key-value pairs currently in the table.
    capacity:              u64                 // The total number of slots in the hash table.
    deleted:               u64                 // The number of slots marked as deleted.
}

impl HashTable
{
    // Returns the number of key-value pairs in the table.
    #[Inline]
    mtd opCount()->u64
    {
        return count
    }

    // Ensures the allocated memory is freed when the table goes out of scope.
    mtd opDrop()
    {
        free()
    }

    // Replaces the content of the table with key-value pairs from a slice.
    mtd opAffect(slice: const [..] { k: K, v: V })
    {
        foreach v in slice:
            add(v.k, v.v)
    }

    // Handles the deep copy of the table when it is copied.
    // A new table is allocated and all entries are copied over.
    mtd opPostCopy()
    {
        if !table:
            return

        if allocator == null:
            allocator = @getcontext().allocator

        let newTable = cast(^HashTableEntry) Memory.alloc(capacity * #sizeof(HashTableEntry), allocator)
        #if Reflection.hasPostCopy(K) or Reflection.hasPostCopy(V)
        {
            for i in capacity
            {
                newTable[i].hash  = table[i].hash
                newTable[i].key   = #nodrop table[i].key
                newTable[i].value = #nodrop table[i].value
            }
        }
        #else
        {
            Memory.copy(newTable, table, capacity * #sizeof(HashTableEntry))
        }

        table = newTable
    }

    // Provides read-only access to the value associated with a key.
    // Asserts if the key is not found.
    #[Overload]
    mtd const opIndex(key: K)->const &V
    {
        #[Global]
        var d: V

        let it = find(key)
        if it:
            return &it.value
        return &d
    }

    // Provides mutable access to the value associated with a key.
    // If the key does not exist, this will exhibit undefined behavior. Use with caution.
    // See [[opIndexAffect]] for safe insertion/update.
    #[Overload]
    mtd opIndex(key: K)->&V
    {
        #[Global]
        var d: V

        let it = tryFind(key)
        if it:
            return &it.value
        return &d
    }

    // Assigns a value to a key, updating it if it exists or adding it if it doesn't.
    mtd opIndexAffect(key: K, val: V)
    {
        let it = tryFind(key)
        if it:
            it.value = val
        else:
            add(key, val)
    }

    // Provides a way to iterate over each key-value pair in the table.
    // This is a macro that accepts a code block to execute for each pair.
    // Aliases: #alias0 for the key, #alias1 for the value.
    #[Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        if !count:
            return

        var remain = count
        #scope(Up)
        for capacity
        {
            if table[@index].hash < HASH_FIRST:
                continue

            #scope(Down)
            #macro
            {
                #if #up ptr
                {
                    let #alias0 = &#up table[@index].key
                    let #alias1 = &#up table[@index].value
                }
                #else
                {
                    #if Reflection.isStruct(K):
                        let #alias0 = cast(const &K) &#up table[@index].key
                    #else:
                        let #alias0 = #up table[@index].key
                    #if Reflection.isStruct(V):
                        let #alias1 = cast(const &V) &#up table[@index].value
                    #else:
                        let #alias1 = #up table[@index].value
                }

                #inject(#up stmt, break = break to Up, continue = break to Down)
            }

            remain -= 1
            if !remain:
                break
        }
    }

    // Computes the hash for a given key.
    // Ensures the resulting hash is not one of the reserved values (FREE, DELETED).
    #[Inline]
    func hashKey(key: K)->u32
    {
        return Hash.hash32(key) | HASH_MASK
    }

    // Frees the hash table's memory and resets its state.
    // If keys or values have destructors, they are called.
    mtd free()
    {
        if !table:
            return

        // Need to drop every structs
        #if Reflection.hasDrop(#typeof(K)) or Reflection.hasDrop(#typeof(V))
        {
            for i in capacity where table[i].hash >= HASH_DELETED
            {
                @drop(&table[i].key)
                @drop(&table[i].value)
            }
        }

        Memory.free(table, capacity * #sizeof(HashTableEntry), allocator)
        table = null
        count, capacity, deleted = 0
    }

    // Checks if the table needs to be resized and triggers it if necessary.
    // Growth happens when the load factor (including deleted slots) exceeds 75%.
    mtd grow()
    {
        // We need to trigger the growing of the table is there's not enough room
        if capacity != 0 and capacity - (count + deleted) > capacity / 4:
            return

        let newCapacity = capacity * 2
        reserve(newCapacity)
    }

    // Resizes the hash table to a new capacity, rehashing all existing elements.
    mtd reserve(newCapacity: u64)
    {
        let reqCapacity = Math.max(newCapacity, 32)
        if reqCapacity <= capacity:
            return

        let oldTable = table
        count, deleted = 0
        let newCapacityBytes = reqCapacity * #sizeof(HashTableEntry)

        if allocator == null:
            allocator = @getcontext().allocator
        table = cast() Memory.alloc(newCapacityBytes, allocator)

        // Be sure hash entry is 0
        Memory.clear(table, newCapacityBytes)

        // Copy the last table to the new table, removing deleted entries
        let oldCapacity = capacity
        capacity = reqCapacity
        for i in oldCapacity
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

        Memory.free(oldTable, oldCapacity * #sizeof(HashTableEntry), allocator)
    }

    // Finds the correct index for a new key, handling collisions and deleted slots.
    // Uses linear probing to find the next available slot.
    mtd getNewIndex(key: K, idx: u64, hashVal: u64)->u64
    {
        var firstFree    = Swag.U64.Max
        var firstDeleted = Swag.U64.Max
        var index        = idx

        while table[index].hash
        {
            if table[index].hash == HASH_DELETED and firstDeleted == Swag.U64.Max:
                firstDeleted = index
            elif table[index].hash == HASH_FREE and firstFree == Swag.U64.Max:
                firstFree = index

            if table[index].hash == hashVal and table[index].key == key:
                return index

            index = (index + 1) % capacity
        }

        if firstDeleted != Swag.U64.Max:
            index = firstDeleted
        elif firstFree != Swag.U64.Max:
            index = firstFree

        return index
    }

    // Adds a new key-value pair to the table by copying them.
    // If the key already exists, the value is updated if 'updateValue' is true.
    #[Discardable, Overload]
    mtd add(key: K, value: V, updateValue = true)->*HashTableEntry
        where Reflection.canCopy(#typeof(K)) and Reflection.canCopy(#typeof(V))
    {
        grow()

        let hashVal = hashKey(key)
        var index   = hashVal % capacity

        index = getNewIndex(key, index, hashVal)
        if table[index].hash >= HASH_FIRST
        {
            if updateValue:
                table[index].value = value
            return table + index
        }

        if table[index].hash == HASH_FREE
        {
            table[index].key   = #nodrop key
            table[index].value = #nodrop value
        }
        else
        {
            if table[index].hash == HASH_DELETED:
                deleted -= 1
            table[index].key   = key
            table[index].value = value
        }

        table[index].hash = hashVal
        count += 1
        return table + index
    }

    // Adds a new key-value pair to the table, moving the value.
    // If the key already exists, the value is updated if 'updateValue' is true.
    #[Discardable, Overload]
    mtd add(key: K, value: &&V, updateValue = true)->*HashTableEntry
        where Reflection.canCopy(#typeof(K))
    {
        grow()

        let hashVal = hashKey(key)
        var index   = hashVal % capacity

        index = getNewIndex(key, index, hashVal)
        if table[index].hash >= HASH_FIRST
        {
            if updateValue:
                table[index].value = #move value
            return table + index
        }

        if table[index].hash == HASH_FREE
        {
            table[index].key   = #nodrop key
            table[index].value = #nodrop #move value
        }
        else
        {
            if table[index].hash == HASH_DELETED:
                deleted -= 1
            table[index].key   = key
            table[index].value = #move value
        }

        table[index].hash = hashVal
        count += 1
        return table + index
    }

    // Internal helper to add a new element with a pre-computed hash. Use with care.
    #[Discardable]
    mtd emplaceInternal(hashVal: u32, key: &K, value: &V)->*HashTableEntry
        where Reflection.canCopy(#typeof(K)) and Reflection.canCopy(#typeof(V))
    {
        grow()

        var index = hashVal % capacity

        index = getNewIndex(key, index, hashVal)
        if table[index].hash >= HASH_FIRST
        {
            table[index].value = #moveraw value
            return table + index
        }

        if table[index].hash == HASH_FREE
        {
            table[index].key   = #nodrop #moveraw key
            table[index].value = #nodrop #moveraw value
        }
        else
        {
            if table[index].hash == HASH_DELETED:
                deleted -= 1
            table[index].key   = #moveraw key
            table[index].value = #moveraw value
        }

        table[index].hash = hashVal
        count += 1
        return table + index
    }

    // Removes a key-value pair from the table by key.
    // If the key exists, its slot is marked as 'DELETED'. Does nothing if the key is not found.
    mtd remove(key: K)
    {
        if !table:
            return

        let hashVal = hashKey(key)
        var index   = hashVal % capacity

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

    // Returns 'true' if the table contains the given key.
    mtd const contains(key: K)->bool
    {
        if !table:
            return false

        let hashVal = hashKey(key)
        var index   = hashVal % capacity

        while table[index].hash
        {
            if table[index].hash == hashVal and table[index].key == key:
                return true
            index = (index + 1) % capacity
        }

        return false
    }

    // Tries to find the entry for a given key.
    // Returns a pointer to the 'HashTableEntry' if found, otherwise returns 'null'.
    mtd const tryFind(key: K)->#null *HashTableEntry
    {
        if !table:
            return null

        let hashVal = hashKey(key)
        var index   = hashVal % capacity

        while table[index].hash
        {
            if table[index].hash == hashVal and table[index].key == key:
                return table + index
            index = (index + 1) % capacity
        }

        return null
    }

    // Finds the entry for a given key.
    // Asserts and will likely crash if the key is not found. Use [[tryFind]] for safe searching.
    mtd const find(key: K)->*HashTableEntry
    {
        Debug.assert(table != null)

        let hashVal = hashKey(key)
        var index   = hashVal % capacity

        while table[index].hash
        {
            if table[index].hash == hashVal and table[index].key == key:
                return table + index
            index = (index + 1) % capacity
        }

        unreachable
    }

    // Gets the value associated with a key.
    // If the key does not exist, returns the provided default value 'val' instead.
    mtd const get(key: K, val: V)->V
    {
        let it = tryFind(key)
        if !it:
            return val
        return it.value
    }

    // Finds the entry for a key, or adds a new entry with a default-initialized value if not found.
    // Returns a pointer to the existing or new entry.
    mtd findOrAdd(key: K)->*HashTableEntry
    {
        let it = tryFind(key)
        if it:
            return it
        var val: V
        return add(key, val)
    }

    // Removes all elements from the table without deallocating the table memory.
    // All slots are marked as 'FREE'.
    mtd clear()
    {
        // Need to put the 'hash' value of each entry to HASH_FREE
        // Drop the key and the value if necessary
        if count or deleted
        {
            for i in capacity
            {
                let hashval = table[i].hash

                if hashval >= HASH_DELETED
                {
                    @drop(&table[i].key)
                    @drop(&table[i].value)
                }

                if hashval == HASH_DELETED:
                    deleted -= 1
                elif hashval >= HASH_FIRST:
                    count -= 1
                table[i].hash = HASH_FREE

                // We are done if there's no more valid value, and no more deleted value
                if !count and !deleted:
                    break
            }
        }
    }
}

impl StrConv.IConvert for HashTable
{
    mtd impl convert(buf: *ConcatBuffer, convFmt: StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes("[")
        foreach k, v in dref self
        {
            if @index:
                buf.addBytes(", ")

            buf.addBytes("key = ")
            StrConv.convert'K(buf, k, convFmt, strFormat)
            buf.addBytes(" value = ")
            StrConv.convert'V(buf, v, convFmt, strFormat)
        }
        buf.addBytes("]")
    }
}
