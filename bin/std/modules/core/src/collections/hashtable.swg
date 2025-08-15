#global marked
#global export
using Swag

// Represents a single key-value pair entry within a HashTable.
struct(K, V) HashTableEntry
{
    hash:      u32 // The cached hash of the key.
    key:       K   // The key.
    value:     V   // The value associated with the key.
}

// A hash self.table implemented with open-addressing that maps keys to values.
// It provides fast lookups, additions, and removals of key-value pairs.
struct(K, V) HashTable
{
    // Special hash values to indicate the state of a slot in the self.table.
    const HASH_FREE        = 0'u32 // The slot is empty and has never been used.
    const HASH_DELETED     = 1'u32 // The slot was previously occupied but has been self.deleted.
    const HASH_FIRST       = 2'u32 // The first possible valid hash value.
    const HASH_MASK        = 2'u32 // A mask to ensure that valid hashes are >= HASH_FIRST.

    allocator:             IAllocator      // The allocator used for memory operations.
    table:                 ^HashTableEntry // Pointer to the contiguous memory block for the hash self.table.
    count:                 u64             // The number of valid key-value pairs currently in the self.table.
    capacity:              u64             // The total number of slots in the hash self.table.
    deleted:               u64             // The number of slots marked as self.deleted.
}

impl HashTable
{
    // Returns the number of key-value pairs in the self.table.
    #[Inline]
    mtd opCount()->u64
    {
        return self.count
    }

    // Ensures the allocated memory is freed when the self.table goes out of scope.
    mtd opDrop()
    {
        self.free()
    }

    // Replaces the content of the self.table with key-value pairs from a slice.
    mtd opAffect(slice: const [..] { k: K, v: V })
    {
        foreach v in slice:
            self.add(v.k, v.v)
    }

    // Handles the deep copy of the self.table when it is copied.
    // A new self.table is allocated and all entries are copied over.
    mtd opPostCopy()
    {
        if !self.table:
            return

        if self.allocator == null:
            self.allocator = @getcontext().allocator

        let newTable = cast(^HashTableEntry) Memory.alloc(self.capacity * #sizeof(HashTableEntry), self.allocator)
        #if Reflection.hasPostCopy(K) or Reflection.hasPostCopy(V)
        {
            for i in self.capacity
            {
                newTable[i].hash  = self.table[i].hash
                newTable[i].key   = #nodrop self.table[i].key
                newTable[i].value = #nodrop self.table[i].value
            }
        }
        #else
        {
            Memory.copy(newTable, self.table, self.capacity * #sizeof(HashTableEntry))
        }

        self.table = newTable
    }

    // Provides read-only access to the value associated with a key.
    // Asserts if the key is not found.
    #[Overload]
    mtd const opIndex(key: K)->const &V
    {
        #[Global]
        var d: V

        let it = self.find(key)
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

        let it = self.tryFind(key)
        if it:
            return &it.value
        return &d
    }

    // Assigns a value to a key, updating it if it exists or adding it if it doesn't.
    mtd opIndexAffect(key: K, val: V)
    {
        let it = self.tryFind(key)
        if it:
            it.value = val
        else:
            self.add(key, val)
    }

    // Provides a way to iterate over each key-value pair in the self.table.
    // This is a macro that accepts a code block to execute for each pair.
    // Aliases: #alias0 for the key, #alias1 for the value.
    #[Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        if !self.count:
            return

        var remain = self.count
        #scope(Up)
        for self.capacity
        {
            if self.table[@index].hash < HashTable.HASH_FIRST:
                continue

            #scope(Down)
            #macro
            {
                #if #up ptr
                {
                    let #alias0 = &#up self.table[@index].key
                    let #alias1 = &#up self.table[@index].value
                }
                #else
                {
                    #if Reflection.isStruct(K):
                        let #alias0 = cast(const &K) &#up self.table[@index].key
                    #else:
                        let #alias0 = #up self.table[@index].key
                    #if Reflection.isStruct(V):
                        let #alias1 = cast(const &V) &#up self.table[@index].value
                    #else:
                        let #alias1 = #up self.table[@index].value
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
        return Hash.hash32(key) | HashTable.HASH_MASK
    }

    // Frees the hash self.table's memory and resets its state.
    // If keys or values have destructors, they are called.
    mtd free()
    {
        if !self.table:
            return

        // Need to drop every structs
        #if Reflection.hasDrop(#typeof(K)) or Reflection.hasDrop(#typeof(V))
        {
            for i in self.capacity where self.table[i].hash >= HashTable.HASH_DELETED
            {
                @drop(&self.table[i].key)
                @drop(&self.table[i].value)
            }
        }

        Memory.free(self.table, self.capacity * #sizeof(HashTableEntry), self.allocator)
        self.table = null
        self.count, self.capacity, self.deleted = 0
    }

    // Checks if the table needs to be resized and triggers it if necessary.
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
        let newCapacityBytes = reqCapacity * #sizeof(HashTableEntry)

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
            if h >= HashTable.HASH_FIRST
            {
                self.emplaceInternal(h, &oldTable[i].key, &oldTable[i].value)
            }
            elif h == HashTable.HASH_DELETED
            {
                @drop(&oldTable[i].key)
                @drop(&oldTable[i].value)
            }
        }

        Memory.free(oldTable, oldCapacity * #sizeof(HashTableEntry), self.allocator)
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
            if self.table[index].hash == HashTable.HASH_DELETED and firstDeleted == Swag.U64.Max:
                firstDeleted = index
            elif self.table[index].hash == HashTable.HASH_FREE and firstFree == Swag.U64.Max:
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

    // Adds a new key-value pair to the self.table by copying them.
    // If the key already exists, the value is updated if 'updateValue' is true.
    #[Discardable, Overload]
    mtd add(key: K, value: V, updateValue = true)->*HashTableEntry
        where Reflection.canCopy(#typeof(K)) and Reflection.canCopy(#typeof(V))
    {
        self.grow()

        let hashVal = hashKey(key)
        var index   = hashVal % self.capacity

        index = self.getNewIndex(key, index, hashVal)
        if self.table[index].hash >= HashTable.HASH_FIRST
        {
            if updateValue:
                self.table[index].value = value
            return self.table + index
        }

        if self.table[index].hash == HashTable.HASH_FREE
        {
            self.table[index].key   = #nodrop key
            self.table[index].value = #nodrop value
        }
        else
        {
            if self.table[index].hash == HashTable.HASH_DELETED:
                self.deleted -= 1
            self.table[index].key   = key
            self.table[index].value = value
        }

        self.table[index].hash = hashVal
        self.count += 1
        return self.table + index
    }

    // Adds a new key-value pair to the self.table, moving the value.
    // If the key already exists, the value is updated if 'updateValue' is true.
    #[Discardable, Overload]
    mtd add(key: K, value: &&V, updateValue = true)->*HashTableEntry
        where Reflection.canCopy(#typeof(K))
    {
        self.grow()

        let hashVal = hashKey(key)
        var index   = hashVal % self.capacity

        index = self.getNewIndex(key, index, hashVal)
        if self.table[index].hash >= HashTable.HASH_FIRST
        {
            if updateValue:
                self.table[index].value = #move value
            return self.table + index
        }

        if self.table[index].hash == HashTable.HASH_FREE
        {
            self.table[index].key   = #nodrop key
            self.table[index].value = #nodrop #move value
        }
        else
        {
            if self.table[index].hash == HashTable.HASH_DELETED:
                self.deleted -= 1
            self.table[index].key   = key
            self.table[index].value = #move value
        }

        self.table[index].hash = hashVal
        self.count += 1
        return self.table + index
    }

    // Internal helper to add a new element with a pre-computed hash. Use with care.
    #[Discardable]
    mtd emplaceInternal(hashVal: u32, key: &K, value: &V)->*HashTableEntry
        where Reflection.canCopy(#typeof(K)) and Reflection.canCopy(#typeof(V))
    {
        self.grow()

        var index = hashVal % self.capacity

        index = self.getNewIndex(key, index, hashVal)
        if self.table[index].hash >= HashTable.HASH_FIRST
        {
            self.table[index].value = #moveraw value
            return self.table + index
        }

        if self.table[index].hash == HashTable.HASH_FREE
        {
            self.table[index].key   = #nodrop #moveraw key
            self.table[index].value = #nodrop #moveraw value
        }
        else
        {
            if self.table[index].hash == HashTable.HASH_DELETED:
                self.deleted -= 1
            self.table[index].key   = #moveraw key
            self.table[index].value = #moveraw value
        }

        self.table[index].hash = hashVal
        self.count += 1
        return self.table + index
    }

    // Removes a key-value pair from the self.table by key.
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
                self.table[index].hash = HashTable.HASH_DELETED
                self.count -= 1
                self.deleted += 1
                return
            }

            index = (index + 1) % self.capacity
        }
    }

    // Returns 'true' if the self.table contains the given key.
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

    // Tries to find the entry for a given key.
    // Returns a pointer to the 'HashTableEntry' if found, otherwise returns 'null'.
    mtd const tryFind(key: K)->#null *HashTableEntry
    {
        if !self.table:
            return null

        let hashVal = hashKey(key)
        var index   = hashVal % self.capacity

        while self.table[index].hash
        {
            if self.table[index].hash == hashVal and self.table[index].key == key:
                return self.table + index
            index = (index + 1) % self.capacity
        }

        return null
    }

    // Finds the entry for a given key.
    // Asserts and will likely crash if the key is not found. Use [[tryFind]] for safe searching.
    mtd const find(key: K)->*HashTableEntry
    {
        Debug.assert(self.table != null)

        let hashVal = hashKey(key)
        var index   = hashVal % self.capacity

        while self.table[index].hash
        {
            if self.table[index].hash == hashVal and self.table[index].key == key:
                return self.table + index
            index = (index + 1) % self.capacity
        }

        unreachable
    }

    // Gets the value associated with a key.
    // If the key does not exist, returns the provided default value 'val' instead.
    mtd const get(key: K, val: V)->V
    {
        let it = self.tryFind(key)
        if !it:
            return val
        return it.value
    }

    // Finds the entry for a key, or adds a new entry with a default-initialized value if not found.
    // Returns a pointer to the existing or new entry.
    mtd findOrAdd(key: K)->*HashTableEntry
    {
        let it = self.tryFind(key)
        if it:
            return it
        var val: V
        return self.add(key, val)
    }

    // Removes all elements from the self.table without deallocating the self.table memory.
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

                if hashval >= HashTable.HASH_DELETED
                {
                    @drop(&self.table[i].key)
                    @drop(&self.table[i].value)
                }

                if hashval == HashTable.HASH_DELETED:
                    self.deleted -= 1
                elif hashval >= HashTable.HASH_FIRST:
                    self.count -= 1
                self.table[i].hash = HashTable.HASH_FREE

                // We are done if there's no more valid value, and no more self.deleted value
                if !self.count and !self.deleted:
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
