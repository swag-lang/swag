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
    const HASH_FREE        = 0'u32                // The slot is empty and has never been used.
    const HASH_DELETED     = 1'u32                // The slot was previously occupied but has been me.deleted.
    const HASH_FIRST       = 2'u32                // The first possible valid hash value.
    const HASH_MASK        = 2'u32                // A mask to ensure that valid hashes are >= HASH_FIRST.

    allocator:             IAllocator             // The me.allocator used for memory operations.
    table:                 [*] HashTableEntry     // Pointer to the contiguous memory block for the hash me.table.
    count:                 u64                    // The number of valid key-value pairs currently in the me.table.
    capacity:              u64                    // The total number of slots in the hash me.table.
    deleted:               u64                    // The number of slots marked as me.deleted.
}

impl HashTable
{
    // Returns the number of key-value pairs in the me.table.
    #[Inline]
    mtd opCount()->u64
    {
        return .count
    }

    // Ensures the allocated memory is freed when the me.table goes out of scope.
    mtd opDrop()
    {
        .free()
    }

    // Replaces the content of the me.table with key-value pairs from a slice.
    mtd opAffect(slice: const [..] { k: K, v: V })
    {
        foreach v in slice do
            .add(v.k, v.v)
    }

    // Handles the deep copy of the me.table when it is copied.
    // A new me.table is allocated and all entries are copied over.
    mtd opPostCopy()
    {
        if !.table do
            return

        if .allocator == null do
            .allocator = @getcontext().allocator

        let newTable = cast([*] HashTableEntry) Memory.alloc(.capacity * #sizeof(HashTableEntry), .allocator)
        #if Reflection.hasPostCopy(K) or Reflection.hasPostCopy(V)
        {
            for i in .capacity
            {
                newTable[i].hash  = .table[i].hash
                newTable[i].key   = #nodrop .table[i].key
                newTable[i].value = #nodrop .table[i].value
            }
        }
        #else
        {
            Memory.copy(newTable, .table, .capacity * #sizeof(HashTableEntry))
        }

        .table = newTable
    }

    // Provides read-only access to the value associated with a key.
    // Asserts if the key is not found.
    #[Overload]
    mtd const opIndex(key: K)->const &V
    {
        #[Global]
        var d: V

        let it = .find(key)
        if it do
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

        let it = .tryFind(key)
        if it do
            return &it.value
        return &d
    }

    // Assigns a value to a key, updating it if it exists or adding it if it doesn't.
    mtd opIndexAffect(key: K, val: V)
    {
        let it = .tryFind(key)
        if it do
            it.value = val
        else do
            .add(key, val)
    }

    // Provides a way to iterate over each key-value pair in the me.table.
    // This is a macro that accepts a code block to execute for each pair.
    // Aliases: #alias0 for the key, #alias1 for the value.
    #[Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        if !.count do
            return

        var remain = .count
        #scope(Up)
        for .capacity
        {
            if .table[@index].hash < HashTable.HASH_FIRST do
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
                    #if Reflection.isStruct(K) do
                        let #alias0 = cast(const &K) &#up me.table[@index].key
                    #else do
                        let #alias0 = #up me.table[@index].key
                    #if Reflection.isStruct(V) do
                        let #alias1 = cast(const &V) &#up me.table[@index].value
                    #else do
                        let #alias1 = #up me.table[@index].value
                }

                #inject(#up stmt, break = break to Up, continue = break to Down)
            }

            remain -= 1
            if !remain do
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
        if !.table do
            return

        // Need to drop every structs
        #if Reflection.hasDrop(#typeof(K)) or Reflection.hasDrop(#typeof(V))
        {
            for i in .capacity where .table[i].hash >= HashTable.HASH_DELETED
            {
                @drop(&.table[i].key)
                @drop(&.table[i].value)
            }
        }

        Memory.free(.table, .capacity * #sizeof(HashTableEntry), .allocator)
        .table = null
        .count, .capacity, .deleted = 0
    }

    // Checks if the me.table needs to be resized and triggers it if necessary.
    // Growth happens when the load factor (including me.deleted slots) exceeds 75%.
    mtd grow()
    {
        // We need to trigger the growing of the me.table is there's not enough room
        if .capacity != 0 and .capacity - (.count + .deleted) > .capacity / 4 do
            return

        let newCapacity = .capacity * 2
        .reserve(newCapacity)
    }

    // Resizes the hash me.table to a new me.capacity, rehashing all existing elements.
    mtd reserve(newCapacity: u64)
    {
        let reqCapacity = Math.max(newCapacity, 32)
        if reqCapacity <= .capacity do
            return

        let oldTable = .table
        .count, .deleted = 0
        let newCapacityBytes = reqCapacity * #sizeof(HashTableEntry)

        if .allocator == null do
            .allocator = @getcontext().allocator
        .table = cast() Memory.alloc(newCapacityBytes, .allocator)

        // Be sure hash entry is 0
        Memory.clear(.table, newCapacityBytes)

        // Copy the last me.table to the new me.table, removing me.deleted entries
        let oldCapacity = .capacity
        .capacity = reqCapacity
        for i in oldCapacity
        {
            let h = oldTable[i].hash
            if h >= HashTable.HASH_FIRST
            {
                .emplaceInternal(h, &oldTable[i].key, &oldTable[i].value)
            }
            elif h == HashTable.HASH_DELETED
            {
                @drop(&oldTable[i].key)
                @drop(&oldTable[i].value)
            }
        }

        Memory.free(oldTable, oldCapacity * #sizeof(HashTableEntry), .allocator)
    }

    // Finds the correct index for a new key, handling collisions and me.deleted slots.
    // Uses linear probing to find the next available slot.
    mtd getNewIndex(key: K, idx: u64, hashVal: u64)->u64
    {
        var firstFree    = Swag.U64.Max
        var firstDeleted = Swag.U64.Max
        var index        = idx

        while .table[index].hash
        {
            if .table[index].hash == HashTable.HASH_DELETED and firstDeleted == Swag.U64.Max do
                firstDeleted = index
            elif .table[index].hash == HashTable.HASH_FREE and firstFree == Swag.U64.Max do
                firstFree = index

            if .table[index].hash == hashVal and .table[index].key == key do
                return index

            index = (index + 1) % .capacity
        }

        if firstDeleted != Swag.U64.Max do
            index = firstDeleted
        elif firstFree != Swag.U64.Max do
            index = firstFree

        return index
    }

    // Adds a new key-value pair to the me.table by copying them.
    // If the key already exists, the value is updated if 'updateValue' is true.
    #[Discardable, Overload]
    mtd add(key: K, value: V, updateValue = true)->*HashTableEntry
        where Reflection.canCopy(#typeof(K)) and Reflection.canCopy(#typeof(V))
    {
        .grow()

        let hashVal = hashKey(key)
        var index   = hashVal % .capacity

        index = .getNewIndex(key, index, hashVal)
        if .table[index].hash >= HashTable.HASH_FIRST
        {
            if updateValue do
                .table[index].value = value
            return .table + index
        }

        if .table[index].hash == HashTable.HASH_FREE
        {
            .table[index].key   = #nodrop key
            .table[index].value = #nodrop value
        }
        else
        {
            if .table[index].hash == HashTable.HASH_DELETED do
                .deleted -= 1
            .table[index].key   = key
            .table[index].value = value
        }

        .table[index].hash = hashVal
        .count += 1
        return .table + index
    }

    // Adds a new key-value pair to the me.table, moving the value.
    // If the key already exists, the value is updated if 'updateValue' is true.
    #[Discardable, Overload]
    mtd add(key: K, value: &&V, updateValue = true)->*HashTableEntry
        where Reflection.canCopy(#typeof(K))
    {
        .grow()

        let hashVal = hashKey(key)
        var index   = hashVal % .capacity

        index = .getNewIndex(key, index, hashVal)
        if .table[index].hash >= HashTable.HASH_FIRST
        {
            if updateValue do
                .table[index].value = #move value
            return .table + index
        }

        if .table[index].hash == HashTable.HASH_FREE
        {
            .table[index].key   = #nodrop key
            .table[index].value = #nodrop #move value
        }
        else
        {
            if .table[index].hash == HashTable.HASH_DELETED do
                .deleted -= 1
            .table[index].key   = key
            .table[index].value = #move value
        }

        .table[index].hash = hashVal
        .count += 1
        return .table + index
    }

    // Internal helper to add a new element with a pre-computed hash. Use with care.
    #[Discardable]
    mtd emplaceInternal(hashVal: u32, key: &K, value: &V)->*HashTableEntry
        where Reflection.canCopy(#typeof(K)) and Reflection.canCopy(#typeof(V))
    {
        .grow()

        var index = hashVal % .capacity

        index = .getNewIndex(key, index, hashVal)
        if .table[index].hash >= HashTable.HASH_FIRST
        {
            .table[index].value = #moveraw value
            return .table + index
        }

        if .table[index].hash == HashTable.HASH_FREE
        {
            .table[index].key   = #nodrop #moveraw key
            .table[index].value = #nodrop #moveraw value
        }
        else
        {
            if .table[index].hash == HashTable.HASH_DELETED do
                .deleted -= 1
            .table[index].key   = #moveraw key
            .table[index].value = #moveraw value
        }

        .table[index].hash = hashVal
        .count += 1
        return .table + index
    }

    // Removes a key-value pair from the me.table by key.
    // If the key exists, its slot is marked as 'DELETED'. Does nothing if the key is not found.
    mtd remove(key: K)
    {
        if !.table do
            return

        let hashVal = hashKey(key)
        var index   = hashVal % .capacity

        while .table[index].hash
        {
            if .table[index].hash == hashVal and .table[index].key == key
            {
                .table[index].hash = HashTable.HASH_DELETED
                .count -= 1
                .deleted += 1
                return
            }

            index = (index + 1) % .capacity
        }
    }

    // Returns 'true' if the me.table contains the given key.
    mtd const contains(key: K)->bool
    {
        if !.table do
            return false

        let hashVal = hashKey(key)
        var index   = hashVal % .capacity

        while .table[index].hash
        {
            if .table[index].hash == hashVal and .table[index].key == key do
                return true
            index = (index + 1) % .capacity
        }

        return false
    }

    // Tries to find the entry for a given key.
    // Returns a pointer to the 'HashTableEntry' if found, otherwise returns 'null'.
    mtd const tryFind(key: K)->#null *HashTableEntry
    {
        if !.table do
            return null

        let hashVal = hashKey(key)
        var index   = hashVal % .capacity

        while .table[index].hash
        {
            if .table[index].hash == hashVal and .table[index].key == key do
                return .table + index
            index = (index + 1) % .capacity
        }

        return null
    }

    // Finds the entry for a given key.
    // Asserts and will likely crash if the key is not found. Use [[tryFind]] for safe searching.
    mtd const find(key: K)->*HashTableEntry
    {
        Debug.assert(.table != null)

        let hashVal = hashKey(key)
        var index   = hashVal % .capacity

        while .table[index].hash
        {
            if .table[index].hash == hashVal and .table[index].key == key do
                return .table + index
            index = (index + 1) % .capacity
        }

        unreachable
    }

    // Gets the value associated with a key.
    // If the key does not exist, returns the provided default value 'val' instead.
    mtd const get(key: K, val: V)->V
    {
        let it = .tryFind(key)
        if !it do
            return val
        return it.value
    }

    // Finds the entry for a key, or adds a new entry with a default-initialized value if not found.
    // Returns a pointer to the existing or new entry.
    mtd findOrAdd(key: K)->*HashTableEntry
    {
        let it = .tryFind(key)
        if it do
            return it
        var val: V
        return .add(key, val)
    }

    // Removes all elements from the me.table without deallocating the me.table memory.
    // All slots are marked as 'FREE'.
    mtd clear()
    {
        // Need to put the 'hash' value of each entry to HASH_FREE
        // Drop the key and the value if necessary
        if .count or .deleted
        {
            for i in .capacity
            {
                let hashval = .table[i].hash

                if hashval >= HashTable.HASH_DELETED
                {
                    @drop(&.table[i].key)
                    @drop(&.table[i].value)
                }

                if hashval == HashTable.HASH_DELETED do
                    .deleted -= 1
                elif hashval >= HashTable.HASH_FIRST do
                    .count -= 1
                .table[i].hash = HashTable.HASH_FREE

                // We are done if there's no more valid value, and no more me.deleted value
                if !.count and !.deleted do
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
            if @index do
                buf.addBytes(", ")

            buf.addBytes("key = ")
            StrConv.convert'K(buf, k, convFmt, strFormat)
            buf.addBytes(" value = ")
            StrConv.convert'V(buf, v, convFmt, strFormat)
        }
        buf.addBytes("]")
    }
}
