#global marked
#global export
using Swag

// Represents a single entry within a HashSet.
struct(K) HashSetEntry
{
    hash:     u32 // The cached hash of the key.
    key:      K   // The key itself.
}

// A hash set implemented with an open-addressing hash self.table.
// It stores a collection of unique keys, providing fast lookups, additions, and removals.
struct(K) HashSet
{
    // Special hash values to indicate the state of a slot in the self.table.
    const HASH_FREE        = 0'u32 // The slot is empty and has never been used.
    const HASH_DELETED     = 1'u32 // The slot was previously occupied but has been self.deleted.
    const HASH_FIRST       = 2'u32 // The first possible valid hash value.
    const HASH_MASK        = 2'u32 // A mask to ensure that valid hashes are >= HASH_FIRST.

    allocator:             IAllocator    // The self.allocator used for memory operations.
    table:                 ^HashSetEntry // Pointer to the contiguous memory block for the hash self.table.
    count:                 u64           // The number of valid elements currently in the set.
    capacity:              u64           // The total number of slots in the hash self.table.
    deleted:               u64           // The number of slots marked as self.deleted.
}

impl HashSet
{
    // Returns the number of elements in the set.
    #[Inline]
    mtd opCount()->u64
    {
        return self.count
    }

    // Ensures the allocated memory is freed when the set goes out of scope.
    mtd opDrop()
    {
        self.free()
    }

    // Replaces the content of the set with elements from a slice.
    mtd opAffect(slice: const [..] K)
    {
        foreach v in slice:
            self.add(v)
    }

    // Handles the deep copy of the set when it is copied.
    // A new self.table is allocated and all entries are copied over.
    mtd opPostCopy()
    {
        if !self.table:
            return

        if self.allocator == null:
            self.allocator = @getcontext().allocator

        let newTable = cast(^HashSetEntry) Memory.alloc(self.capacity * #sizeof(HashSetEntry), self.allocator)
        #if Reflection.hasPostCopy(K)
        {
            for i in self.capacity
            {
                newTable[i].hash = self.table[i].hash
                newTable[i].key  = #nodrop self.table[i].key
            }
        }
        #else
        {
            Memory.copy(newTable, self.table, self.capacity * #sizeof(HashSetEntry))
        }

        self.table = newTable
    }

    // Provides a way to iterate over each key in the set.
    // This is a macro that accepts a code block to execute for each key.
    // Alias: #alias0 for the key or a pointer to it.
    #[Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        if !self.count:
            return

        var remain = self.count
        #scope(Up)
        for self.capacity
        {
            if self.table[@index].hash < HashSet.HASH_FIRST:
                continue

            #scope(Down)
            #macro
            {
                #if #up ptr
                {
                    let #alias0 = &#up self.table[@index].key
                }
                #elif Reflection.isStruct(K)
                {
                    let #alias0 = cast(const &K) &#up self.table[@index].key
                }
                #else
                {
                    let #alias0 = #up self.table[@index].key
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
        return Hash.hash32(key) | HashSet.HASH_MASK
    }

    // Frees the hash set's memory and resets its state.
    // If keys have destructors, they are called.
    mtd free()
    {
        if !self.table:
            return

        // Need to drop every structs
        #if Reflection.hasDrop(#typeof(K))
        {
            for i in self.capacity
            {
                if self.table[i].hash >= HashSet.HASH_DELETED:
                    @drop(&self.table[i].key)
            }
        }

        Memory.free(self.table, self.capacity * #sizeof(HashSetEntry), self.allocator)
        self.table = null
        self.count, self.capacity, self.deleted = 0
    }

    // Checks if the self.table needs to be resized and triggers it if necessary.
    // Growth happens when the load factor (including self.deleted slots) exceeds 75%.
    mtd grow()
    {
        // We need to trigger the growing of the self.table is there's not enough room
        if self.capacity != 0 and self.capacity - (self.count + self.deleted) > self.capacity / 4:
            return

        let newCapacity = self.capacity * 2
        self.reserve(newCapacity)
    }

    // Resizes the hash self.table to a new self.capacity, rehashing all existing elements.
    mtd reserve(newCapacity: u64)
    {
        let reqCapacity = Math.max(newCapacity, 32)
        if reqCapacity <= self.capacity:
            return

        let oldTable = self.table
        self.count, self.deleted = 0
        let newCapacityBytes = reqCapacity * #sizeof(HashSetEntry)

        if self.allocator == null:
            self.allocator = @getcontext().allocator
        self.table = cast() Memory.alloc(newCapacityBytes, self.allocator)

        // Be sure hash entry is 0
        Memory.clear(self.table, newCapacityBytes)

        // Copy the last self.table to the new self.table, removing self.deleted entries
        let oldCapacity = self.capacity
        self.capacity = reqCapacity
        for i in oldCapacity
        {
            let h = oldTable[i].hash
            if h >= HashSet.HASH_FIRST
            {
                self.emplaceInternal(h, &oldTable[i].key)
            }
            elif h == HashSet.HASH_DELETED
            {
                @drop(&oldTable[i].key)
            }
        }

        Memory.free(oldTable, oldCapacity * #sizeof(HashSetEntry), self.allocator)
    }

    // Finds the correct index for a new key, handling collisions and self.deleted slots.
    // Uses linear probing to find the next available slot.
    mtd getNewIndex(key: K, idx: u64, hashVal: u64)->u64
    {
        var firstFree    = Swag.U64.Max
        var firstDeleted = Swag.U64.Max
        var index        = idx

        while self.table[index].hash
        {
            if self.table[index].hash == HashSet.HASH_DELETED and firstDeleted == Swag.U64.Max:
                firstDeleted = index
            elif self.table[index].hash == HashSet.HASH_FREE and firstFree == Swag.U64.Max:
                firstFree = index

            if self.table[index].hash == hashVal and self.table[index].key == key:
                return index

            index = (index + 1) % self.capacity
        }

        if firstDeleted != Swag.U64.Max:
            index = firstDeleted
        elif firstFree != Swag.U64.Max:
            index = firstFree

        return index
    }

    // Adds a new key to the set by copying it.
    // If the key already exists, does nothing. Returns a pointer to the entry.
    #[Discardable, Overload]
    mtd add(key: K)->*HashSetEntry
        where Reflection.canCopy(#typeof(K))
    {
        self.grow()

        let hashVal = hashKey(key)
        var index   = hashVal % self.capacity

        index = self.getNewIndex(key, index, hashVal)
        if self.table[index].hash >= HashSet.HASH_FIRST:
            return self.table + index

        if self.table[index].hash == HashSet.HASH_FREE
        {
            self.table[index].key = #nodrop key
        }
        else
        {
            if self.table[index].hash == HashSet.HASH_DELETED:
                self.deleted -= 1
            self.table[index].key = key
        }

        self.table[index].hash = hashVal
        self.count += 1
        return self.table + index
    }

    // Adds a new key to the set by moving it.
    // If the key already exists, does nothing. Returns a pointer to the entry.
    #[Discardable, Overload]
    mtd add(key: &&K)->*HashSetEntry
    {
        self.grow()

        let hashVal = hashKey(key)
        var index   = hashVal % self.capacity

        index = self.getNewIndex(key, index, hashVal)
        if self.table[index].hash >= HashSet.HASH_FIRST:
            return self.table + index

        if self.table[index].hash == HashSet.HASH_FREE
        {
            self.table[index].key = #nodrop #move key
        }
        else
        {
            if self.table[index].hash == HashSet.HASH_DELETED:
                self.deleted -= 1
            self.table[index].key = #move key
        }

        self.table[index].hash = hashVal
        self.count += 1
        return self.table + index
    }

    // Internal helper to add a new element with a pre-computed hash.
    #[Discardable]
    mtd emplaceInternal(hashVal: u32, key: &K)->*HashSetEntry
        where Reflection.canCopy(#typeof(K))
    {
        self.grow()

        var index = hashVal % self.capacity

        index = self.getNewIndex(key, index, hashVal)
        if self.table[index].hash >= HashSet.HASH_FIRST:
            return self.table + index

        if self.table[index].hash == HashSet.HASH_FREE
        {
            self.table[index].key = #nodrop #moveraw key
        }
        else
        {
            if self.table[index].hash == HashSet.HASH_DELETED:
                self.deleted -= 1
            self.table[index].key = #moveraw key
        }

        self.table[index].hash = hashVal
        self.count += 1
        return self.table + index
    }

    // Removes a key from the set.
    // If the key exists, its slot is marked as 'DELETED'. Does nothing if the key is not found.
    mtd remove(key: K)
    {
        if !self.table:
            return

        let hashVal = hashKey(key)
        var index   = hashVal % self.capacity

        while self.table[index].hash
        {
            if self.table[index].hash == hashVal and self.table[index].key == key
            {
                self.table[index].hash = HashSet.HASH_DELETED
                self.count -= 1
                self.deleted += 1
                return
            }

            index = (index + 1) % self.capacity
        }
    }

    // Returns 'true' if the set contains the given key.
    mtd const contains(key: K)->bool
    {
        if !self.table:
            return false

        let hashVal = hashKey(key)
        var index   = hashVal % self.capacity

        while self.table[index].hash
        {
            if self.table[index].hash == hashVal and self.table[index].key == key:
                return true
            index = (index + 1) % self.capacity
        }

        return false
    }

    // Removes all elements from the set without deallocating the self.table memory.
    // All slots are marked as 'FREE'.
    mtd clear()
    {
        // Need to put the 'hash' value of each entry to HASH_FREE
        // Drop the key and the value if necessary
        if self.count or self.deleted
        {
            for i in self.capacity
            {
                let hashval = self.table[i].hash
                if hashval >= HashSet.HASH_DELETED:
                    @drop(&self.table[i].key)

                if hashval == HashSet.HASH_DELETED:
                    self.deleted -= 1
                elif hashval >= HashSet.HASH_FIRST:
                    self.count -= 1
                self.table[i].hash = HashSet.HASH_FREE

                // We are done if there's no more valid value, and no more self.deleted value
                if !self.count and !self.deleted:
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
        foreach k in dref self
        {
            if @index:
                buf.addBytes(", ")
            StrConv.convert'K(buf, k, convFormat, strFormat)
        }
        buf.addBytes("]")
    }
}
