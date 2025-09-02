#global export
using Swag

// Represents a single key-value pair entry within a HashTable.
struct(K, V) HashTableEntry
{
    hash:      u32     // The cached hash of the key.
    key:       K       // The key.
    value:     V       // The value associated with the key.
}

// A hash me.table implemented with open-addressing that maps keys to values.
// It provides fast lookups, additions, and removals of key-value pairs.
struct(K, V) HashTable
{
    // Special hash values to indicate the state of a slot in the me.table.
    const HASH_FREE        = 0'u32             // The slot is empty and has never been used.
    const HASH_DELETED     = 1'u32             // The slot was previously occupied but has been me.deleted.
    const HASH_FIRST       = 2'u32             // The first possible valid hash value.
    const HASH_MASK        = 2'u32             // A mask to ensure that valid hashes are >= HASH_FIRST.

    allocator:             IAllocator          // The me.allocator used for memory operations.
    table:                 ^HashTableEntry     // Pointer to the contiguous memory block for the hash me.table.
    count:                 u64                 // The number of valid key-value pairs currently in the me.table.
    capacity:              u64                 // The total number of slots in the hash me.table.
    deleted:               u64                 // The number of slots marked as me.deleted.
}

impl HashTable
{
    // Returns the number of key-value pairs in the me.table.
    #[Inline]
    mtd opCount()->u64
    {
        return me.count
    }

    // Ensures the allocated memory is freed when the me.table goes out of scope.
    mtd opDrop()
    {
        me.free()
    }

    // Replaces the content of the me.table with key-value pairs from a slice.
    mtd opAffect(slice: const [..] { k: K, v: V })
    {
        foreach v in slice:
            me.add(v.k, v.v)
    }

    // Handles the deep copy of the me.table when it is copied.
    // A new me.table is allocated and all entries are copied over.
    mtd opPostCopy()
    {
        if !me.table:
            return

        if me.allocator == null:
            me.allocator = @getcontext().allocator

        let newTable = cast(^HashTableEntry) Memory.alloc(me.capacity * #sizeof(HashTableEntry), me.allocator)
        #if Reflection.hasPostCopy(K) or Reflection.hasPostCopy(V)
        {
            for i in me.capacity
            {
                newTable[i].hash  = me.table[i].hash
                newTable[i].key   = #nodrop me.table[i].key
                newTable[i].value = #nodrop me.table[i].value
            }
        }
        #else
        {
            Memory.copy(newTable, me.table, me.capacity * #sizeof(HashTableEntry))
        }

        me.table = newTable
    }

    // Provides read-only access to the value associated with a key.
    // Asserts if the key is not found.
    #[Overload]
    mtd const opIndex(key: K)->const &V
    {
        #[Global]
        var d: V

        let it = me.find(key)
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

        let it = me.tryFind(key)
        if it:
            return &it.value
        return &d
    }

    // Assigns a value to a key, updating it if it exists or adding it if it doesn't.
    mtd opIndexAffect(key: K, val: V)
    {
        let it = me.tryFind(key)
        if it:
            it.value = val
        else:
            me.add(key, val)
    }

    // Provides a way to iterate over each key-value pair in the me.table.
    // This is a macro that accepts a code block to execute for each pair.
    // Aliases: #alias0 for the key, #alias1 for the value.
    #[Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        if !me.count:
            return

        var remain = me.count
        #scope(Up)
        for me.capacity
        {
            if me.table[@index].hash < HashTable.HASH_FIRST:
                continue

            #scope(Down)
            #macro
            {
                #if #up ptr
                {
                    let #alias0 = &#up me.table[@index].key
                    let #alias1 = &#up me.table[@index].value
                }
                #else
                {
                    #if Reflection.isStruct(K):
                        let #alias0 = cast(const &K) &#up me.table[@index].key
                    #else:
                        let #alias0 = #up me.table[@index].key
                    #if Reflection.isStruct(V):
                        let #alias1 = cast(const &V) &#up me.table[@index].value
                    #else:
                        let #alias1 = #up me.table[@index].value
                }

                #inject(#up stmt, break = break to Up, continue = break to Down)
            }

            remain -= 1
            if !remain:
                break
        }
    }

    // Computes the hash for a given key.
    // Ensures the resulting hash is not one of the reserved values (FREE, me.deleted).
    #[Inline]
    func hashKey(key: K)->u32
    {
        return Hash.hash32(key) | HashTable.HASH_MASK
    }

    // Frees the hash me.table's memory and resets its state.
    // If keys or values have destructors, they are called.
    mtd free()
    {
        if !me.table:
            return

        // Need to drop every structs
        #if Reflection.hasDrop(#typeof(K)) or Reflection.hasDrop(#typeof(V))
        {
            for i in me.capacity where me.table[i].hash >= HashTable.HASH_DELETED
            {
                @drop(&me.table[i].key)
                @drop(&me.table[i].value)
            }
        }

        Memory.free(me.table, me.capacity * #sizeof(HashTableEntry), me.allocator)
        me.table = null
        me.count, me.capacity, me.deleted = 0
    }

    // Checks if the me.table needs to be resized and triggers it if necessary.
    // Growth happens when the load factor (including me.deleted slots) exceeds 75%.
    mtd grow()
    {
        // We need to trigger the growing of the me.table is there's not enough room
        if me.capacity != 0 and me.capacity - (me.count + me.deleted) > me.capacity / 4:
            return

        let newCapacity = me.capacity * 2
        me.reserve(newCapacity)
    }

    // Resizes the hash me.table to a new me.capacity, rehashing all existing elements.
    mtd reserve(newCapacity: u64)
    {
        let reqCapacity = Math.max(newCapacity, 32)
        if reqCapacity <= me.capacity:
            return

        let oldTable = me.table
        me.count, me.deleted = 0
        let newCapacityBytes = reqCapacity * #sizeof(HashTableEntry)

        if me.allocator == null:
            me.allocator = @getcontext().allocator
        me.table = cast() Memory.alloc(newCapacityBytes, me.allocator)

        // Be sure hash entry is 0
        Memory.clear(me.table, newCapacityBytes)

        // Copy the last me.table to the new me.table, removing me.deleted entries
        let oldCapacity = me.capacity
        me.capacity = reqCapacity
        for i in oldCapacity
        {
            let h = oldTable[i].hash
            if h >= HashTable.HASH_FIRST
            {
                me.emplaceInternal(h, &oldTable[i].key, &oldTable[i].value)
            }
            elif h == HashTable.HASH_DELETED
            {
                @drop(&oldTable[i].key)
                @drop(&oldTable[i].value)
            }
        }

        Memory.free(oldTable, oldCapacity * #sizeof(HashTableEntry), me.allocator)
    }

    // Finds the correct index for a new key, handling collisions and me.deleted slots.
    // Uses linear probing to find the next available slot.
    mtd getNewIndex(key: K, idx: u64, hashVal: u64)->u64
    {
        var firstFree    = Swag.U64.Max
        var firstDeleted = Swag.U64.Max
        var index        = idx

        while me.table[index].hash
        {
            if me.table[index].hash == HashTable.HASH_DELETED and firstDeleted == Swag.U64.Max:
                firstDeleted = index
            elif me.table[index].hash == HashTable.HASH_FREE and firstFree == Swag.U64.Max:
                firstFree = index

            if me.table[index].hash == hashVal and me.table[index].key == key:
                return index

            index = (index + 1) % me.capacity
        }

        if firstDeleted != Swag.U64.Max:
            index = firstDeleted
        elif firstFree != Swag.U64.Max:
            index = firstFree

        return index
    }

    // Adds a new key-value pair to the me.table by copying them.
    // If the key already exists, the value is updated if 'updateValue' is true.
    #[Discardable, Overload]
    mtd add(key: K, value: V, updateValue = true)->*HashTableEntry
        where Reflection.canCopy(#typeof(K)) and Reflection.canCopy(#typeof(V))
    {
        me.grow()

        let hashVal = hashKey(key)
        var index   = hashVal % me.capacity

        index = me.getNewIndex(key, index, hashVal)
        if me.table[index].hash >= HashTable.HASH_FIRST
        {
            if updateValue:
                me.table[index].value = value
            return me.table + index
        }

        if me.table[index].hash == HashTable.HASH_FREE
        {
            me.table[index].key   = #nodrop key
            me.table[index].value = #nodrop value
        }
        else
        {
            if me.table[index].hash == HashTable.HASH_DELETED:
                me.deleted -= 1
            me.table[index].key   = key
            me.table[index].value = value
        }

        me.table[index].hash = hashVal
        me.count += 1
        return me.table + index
    }

    // Adds a new key-value pair to the me.table, moving the value.
    // If the key already exists, the value is updated if 'updateValue' is true.
    #[Discardable, Overload]
    mtd add(key: K, value: &&V, updateValue = true)->*HashTableEntry
        where Reflection.canCopy(#typeof(K))
    {
        me.grow()

        let hashVal = hashKey(key)
        var index   = hashVal % me.capacity

        index = me.getNewIndex(key, index, hashVal)
        if me.table[index].hash >= HashTable.HASH_FIRST
        {
            if updateValue:
                me.table[index].value = #move value
            return me.table + index
        }

        if me.table[index].hash == HashTable.HASH_FREE
        {
            me.table[index].key   = #nodrop key
            me.table[index].value = #nodrop #move value
        }
        else
        {
            if me.table[index].hash == HashTable.HASH_DELETED:
                me.deleted -= 1
            me.table[index].key   = key
            me.table[index].value = #move value
        }

        me.table[index].hash = hashVal
        me.count += 1
        return me.table + index
    }

    // Internal helper to add a new element with a pre-computed hash. Use with care.
    #[Discardable]
    mtd emplaceInternal(hashVal: u32, key: &K, value: &V)->*HashTableEntry
        where Reflection.canCopy(#typeof(K)) and Reflection.canCopy(#typeof(V))
    {
        me.grow()

        var index = hashVal % me.capacity

        index = me.getNewIndex(key, index, hashVal)
        if me.table[index].hash >= HashTable.HASH_FIRST
        {
            me.table[index].value = #moveraw value
            return me.table + index
        }

        if me.table[index].hash == HashTable.HASH_FREE
        {
            me.table[index].key   = #nodrop #moveraw key
            me.table[index].value = #nodrop #moveraw value
        }
        else
        {
            if me.table[index].hash == HashTable.HASH_DELETED:
                me.deleted -= 1
            me.table[index].key   = #moveraw key
            me.table[index].value = #moveraw value
        }

        me.table[index].hash = hashVal
        me.count += 1
        return me.table + index
    }

    // Removes a key-value pair from the me.table by key.
    // If the key exists, its slot is marked as 'DELETED'. Does nothing if the key is not found.
    mtd remove(key: K)
    {
        if !me.table:
            return

        let hashVal = hashKey(key)
        var index   = hashVal % me.capacity

        while me.table[index].hash
        {
            if me.table[index].hash == hashVal and me.table[index].key == key
            {
                me.table[index].hash = HashTable.HASH_DELETED
                me.count -= 1
                me.deleted += 1
                return
            }

            index = (index + 1) % me.capacity
        }
    }

    // Returns 'true' if the me.table contains the given key.
    mtd const contains(key: K)->bool
    {
        if !me.table:
            return false

        let hashVal = hashKey(key)
        var index   = hashVal % me.capacity

        while me.table[index].hash
        {
            if me.table[index].hash == hashVal and me.table[index].key == key:
                return true
            index = (index + 1) % me.capacity
        }

        return false
    }

    // Tries to find the entry for a given key.
    // Returns a pointer to the 'HashTableEntry' if found, otherwise returns 'null'.
    mtd const tryFind(key: K)->#null *HashTableEntry
    {
        if !me.table:
            return null

        let hashVal = hashKey(key)
        var index   = hashVal % me.capacity

        while me.table[index].hash
        {
            if me.table[index].hash == hashVal and me.table[index].key == key:
                return me.table + index
            index = (index + 1) % me.capacity
        }

        return null
    }

    // Finds the entry for a given key.
    // Asserts and will likely crash if the key is not found. Use [[tryFind]] for safe searching.
    mtd const find(key: K)->*HashTableEntry
    {
        Debug.assert(me.table != null)

        let hashVal = hashKey(key)
        var index   = hashVal % me.capacity

        while me.table[index].hash
        {
            if me.table[index].hash == hashVal and me.table[index].key == key:
                return me.table + index
            index = (index + 1) % me.capacity
        }

        unreachable
    }

    // Gets the value associated with a key.
    // If the key does not exist, returns the provided default value 'val' instead.
    mtd const get(key: K, val: V)->V
    {
        let it = me.tryFind(key)
        if !it:
            return val
        return it.value
    }

    // Finds the entry for a key, or adds a new entry with a default-initialized value if not found.
    // Returns a pointer to the existing or new entry.
    mtd findOrAdd(key: K)->*HashTableEntry
    {
        let it = me.tryFind(key)
        if it:
            return it
        var val: V
        return me.add(key, val)
    }

    // Removes all elements from the me.table without deallocating the me.table memory.
    // All slots are marked as 'FREE'.
    mtd clear()
    {
        // Need to put the 'hash' value of each entry to HASH_FREE
        // Drop the key and the value if necessary
        if me.count or me.deleted
        {
            for i in me.capacity
            {
                let hashval = me.table[i].hash

                if hashval >= HashTable.HASH_DELETED
                {
                    @drop(&me.table[i].key)
                    @drop(&me.table[i].value)
                }

                if hashval == HashTable.HASH_DELETED:
                    me.deleted -= 1
                elif hashval >= HashTable.HASH_FIRST:
                    me.count -= 1
                me.table[i].hash = HashTable.HASH_FREE

                // We are done if there's no more valid value, and no more me.deleted value
                if !me.count and !me.deleted:
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
        foreach k, v in dref me
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
