#global export
using Swag

// Represents a single entry within a HashSet.
struct(K) HashSetEntry
{
    hash:     u32     // The cached hash of the key.
    key:      K       // The key itself.
}

// A hash set implemented with an open-addressing hash me.table.
// It stores a collection of unique keys, providing fast lookups, additions, and removals.
struct(K) HashSet
{
    // Special hash values to indicate the state of a slot in the me.table.
    const HASH_FREE        = 0'u32              // The slot is empty and has never been used.
    const HASH_DELETED     = 1'u32              // The slot was previously occupied but has been me.deleted.
    const HASH_FIRST       = 2'u32              // The first possible valid hash value.
    const HASH_MASK        = 2'u32              // A mask to ensure that valid hashes are >= HASH_FIRST.

    allocator:             IAllocator           // The me.allocator used for memory operations.
    table:                 [*] HashSetEntry     // Pointer to the contiguous memory block for the hash me.table.
    count:                 u64                  // The number of valid elements currently in the set.
    capacity:              u64                  // The total number of slots in the hash me.table.
    deleted:               u64                  // The number of slots marked as deleted.
}

impl HashSet
{
    // Returns the number of elements in the set.
    #[Inline]
    mtd opCount()->u64
    {
        return .count
    }

    // Ensures the allocated memory is freed when the set goes out of scope.
    mtd opDrop()
    {
        .free()
    }

    // Replaces the content of the set with elements from a slice.
    mtd opAffect(slice: const [..] K)
    {
        foreach v in slice do
            .add(v)
    }

    // Handles the deep copy of the set when it is copied.
    // A new me.table is allocated and all entries are copied over.
    mtd opPostCopy()
    {
        if !.table do
            return

        if .allocator == null do
            .allocator = @getcontext().allocator

        let newTable = cast([*] HashSetEntry) Memory.alloc(.capacity * #sizeof(HashSetEntry), .allocator)
        #if Reflection.hasPostCopy(K)
        {
            for i in .capacity
            {
                newTable[i].hash = .table[i].hash
                newTable[i].key  = #nodrop .table[i].key
            }
        }
        #else
        {
            Memory.copy(newTable, .table, .capacity * #sizeof(HashSetEntry))
        }

        .table = newTable
    }

    // Provides a way to iterate over each key in the set.
    // This is a macro that accepts a code block to execute for each key.
    // Alias: #alias0 for the key or a pointer to it.
    #[Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        if !.count do
            return

        var remain = .count
        #scope(Up)
        for .capacity
        {
            if .table[@index].hash < HashSet.HASH_FIRST do
                continue

            #scope(Down)
            #macro
            {
                #if #up ptr
                {
                    let #alias0 = &#up me.table[@index].key
                }
                #elif Reflection.isStruct(K)
                {
                    let #alias0 = cast(const &K) &#up me.table[@index].key
                }
                #else
                {
                    let #alias0 = #up me.table[@index].key
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
        return Hash.hash32(key) | HashSet.HASH_MASK
    }

    // Frees the hash set's memory and resets its state.
    // If keys have destructors, they are called.
    mtd free()
    {
        if !.table do
            return

        // Need to drop every structs
        #if Reflection.hasDrop(#typeof(K))
        {
            for i in .capacity
            {
                if .table[i].hash >= HashSet.HASH_DELETED do
                    @drop(&.table[i].key)
            }
        }

        Memory.free(.table, .capacity * #sizeof(HashSetEntry), .allocator)
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
        let newCapacityBytes = reqCapacity * #sizeof(HashSetEntry)

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
            if h >= HashSet.HASH_FIRST
            {
                .emplaceInternal(h, &oldTable[i].key)
            }
            elif h == HashSet.HASH_DELETED
            {
                @drop(&oldTable[i].key)
            }
        }

        Memory.free(oldTable, oldCapacity * #sizeof(HashSetEntry), .allocator)
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
            if .table[index].hash == HashSet.HASH_DELETED and firstDeleted == Swag.U64.Max do
                firstDeleted = index
            elif .table[index].hash == HashSet.HASH_FREE and firstFree == Swag.U64.Max do
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

    // Adds a new key to the set by copying it.
    // If the key already exists, does nothing. Returns a pointer to the entry.
    #[Discardable, Overload]
    mtd add(key: K)->*HashSetEntry
        where Reflection.canCopy(#typeof(K))
    {
        .grow()

        let hashVal = hashKey(key)
        var index   = hashVal % .capacity

        index = .getNewIndex(key, index, hashVal)
        if .table[index].hash >= HashSet.HASH_FIRST do
            return .table + index

        if .table[index].hash == HashSet.HASH_FREE
        {
            .table[index].key = #nodrop key
        }
        else
        {
            if .table[index].hash == HashSet.HASH_DELETED do
                .deleted -= 1
            .table[index].key = key
        }

        .table[index].hash = hashVal
        .count += 1
        return .table + index
    }

    // Adds a new key to the set by moving it.
    // If the key already exists, does nothing. Returns a pointer to the entry.
    #[Discardable, Overload]
    mtd add(key: &&K)->*HashSetEntry
    {
        .grow()

        let hashVal = hashKey(key)
        var index   = hashVal % .capacity

        index = .getNewIndex(key, index, hashVal)
        if .table[index].hash >= HASH_FIRST do
            return .table + index

        if .table[index].hash == HASH_FREE
        {
            .table[index].key = #nodrop #move key
        }
        else
        {
            if .table[index].hash == HASH_DELETED do
                .deleted -= 1
            .table[index].key = #move key
        }

        .table[index].hash = hashVal
        .count += 1
        return .table + index
    }

    // Internal helper to add a new element with a pre-computed hash.
    #[Discardable]
    mtd emplaceInternal(hashVal: u32, key: &K)->*HashSetEntry
        where Reflection.canCopy(#typeof(K))
    {
        .grow()

        var index = hashVal % .capacity

        index = .getNewIndex(key, index, hashVal)
        if .table[index].hash >= HashSet.HASH_FIRST do
            return .table + index

        if .table[index].hash == HashSet.HASH_FREE
        {
            .table[index].key = #nodrop #moveraw key
        }
        else
        {
            if .table[index].hash == HashSet.HASH_DELETED do
                .deleted -= 1
            .table[index].key = #moveraw key
        }

        .table[index].hash = hashVal
        .count += 1
        return .table + index
    }

    // Removes a key from the set.
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
                .table[index].hash = HashSet.HASH_DELETED
                .count -= 1
                .deleted += 1
                return
            }

            index = (index + 1) % .capacity
        }
    }

    // Returns 'true' if the set contains the given key.
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

    // Removes all elements from the set without deallocating the me.table memory.
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
                if hashval >= HashSet.HASH_DELETED do
                    @drop(&.table[i].key)

                if hashval == HashSet.HASH_DELETED do
                    .deleted -= 1
                elif hashval >= HashSet.HASH_FIRST do
                    .count -= 1
                .table[i].hash = HashSet.HASH_FREE

                // We are done if there's no more valid value, and no more me.deleted value
                if !.count and !.deleted do
                    break
            }
        }
    }
}

impl StrConv.IConvert for HashSet
{
    mtd impl convert(buf: *ConcatBuffer, convFormat: StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes("[")
        foreach k in dref me
        {
            if @index do
                buf.addBytes(", ")
            StrConv.convert'K(buf, k, convFormat, strFormat)
        }
        buf.addBytes("]")
    }
}
