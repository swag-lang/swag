using Swag

// Manages a compact array of bit values, which are represented as booleans, where 'true' indicates that
// the bit is on (1) and 'false' indicates the bit is off (0)
public struct BitArray
{
    buffer:        ^u32
    allocator:     IAllocator
    count:         u64
    capacity:      u64
}

impl BitArray
{
    mtd free()
    {
        if !buffer:
            return
        Memory.free(buffer, capacity, allocator)
    }

    mtd realloc(newCapacity: u64)
    {
        if allocator == null:
            allocator = @getcontext().allocator
        buffer = cast() Memory.realloc(buffer, newCapacity, capacity, allocator)
    }
}

public impl BitArray
{
    mtd const opCount()->u64
    {
        return count
    }

    mtd opDrop()
    {
        free()
    }

    mtd opPostCopy()
    {
        if buffer
        {
            let otherBuffer = buffer
            buffer = null
            reserve(count)
            Memory.copy(buffer, otherBuffer, capacity)
        }
    }

    // Gets the value of the bit at a specific position
    mtd opIndex(index: u64)->bool
    {
        return get(index)
    }

    // Sets the value of the bit at a specific position
    mtd opIndexAffect(index: u64, value: bool)
    {
        set(index, value)
    }

    // Initializes a 'BitArray' that contains bit values copied from the specified array of booleans
    mtd opAffect(what: const [..] bool)
    {
        let len = @countof(what)
        reserve(len)
        buffer[len >> 5] = 0
        foreach value, index in what:
            set(index, value)
    }

    // Compares two arrays, and returns 'true' if they are equal
    mtd opEquals(other: Self)->bool
    {
        if count != other.count:
            return false

        for Math.max(count >> 5, 1)
        {
            if buffer[#index] != other.buffer[#index]:
                return false
        }

        return true
    }

    // Visit all the booleans
    #[Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        #if ptr:
            #error "visiting by pointer is not supported for BitArray"
        #if back:
            #error "visiting in reverse order is not supported for BitArray"

        if !count:
            return
        let remainingBits = count
        var countBits     = 0'u64

        #scope(Up)
        for idx in Math.max(count >> 5, 1)
        {
            var shift: u32 = 1
            for Math.min(remainingBits, 32)
            {
                #scope(Down)
                #macro
                {
                    let #alias0 = cast(bool) (#up buffer[#up idx] & #up shift)
                    let #alias1 = #up countBits
                    #inject(#up stmt, break = break to Up, continue = break to Down)
                }

                countBits += 1
                shift <<= 1
            }
        }
    }

    // Reserve the given amount of bits.
    // The bits will **not** be initialized.
    #[Swag.Overload]
    mtd reserve(maxBits: u64)
    {
        if !maxBits
        {
            free()
            buffer = null
            count, capacity = 0
            return
        }

        count    = maxBits
        capacity = maxBits >> 5
        if capacity << 5 < maxBits:
            capacity += 1

        capacity *= 4
        realloc(capacity)
    }

    // Reserve the given amount of bits and set an initial value to all bits.
    #[Swag.Overload]
    mtd reserve(maxBits: u64, initialValue: bool)
    {
        reserve(maxBits)
        Memory.set(buffer, initialValue ? 0xFF : 0x00, capacity)
    }

    // Sets all bits to the specified value
    mtd setAll(value: bool)
    {
        Memory.set(buffer, value ? 0xFF : 0x00, capacity)
    }

    // Sets the bit at a specific position to the specified value
    mtd set(index: u64, value: bool)
    {
        Debug.assert(index < count)
        let val   = index >> 5
        let bit   = index - (val << 5)
        let shift = 1'u32 << cast(u32) bit
        if value:
            buffer[val] |= shift
        else:
            buffer[val] &= ~shift
    }

    // Gets the value of the bit at a specific position
    mtd const get(index: u64)->bool
    {
        Debug.assert(index < count)
        let val   = index >> 5
        let bit   = index - (val << 5)
        let shift = 1'u32 << cast(u32) bit
        return cast(bool) (buffer[val] & shift)
    }

    // Inverts the value of the bit at a specific position
    #[Swag.Overload]
    mtd invert(index: u64)
    {
        Debug.assert(index < count)
        let val   = index >> 5
        let bit   = index - (val << 5)
        let shift = 1'u32 << cast(u32) bit
        if buffer[val] & shift:
            buffer[val] ^= shift
        else:
            buffer[val] |= shift
    }

    // Inverts all the bit values, so that elements set to 'true' are changed to 'false', and elements set to 'false' are changed to 'true'.
    #[Swag.Overload]
    mtd invert()
    {
        if !count:
            return
        for Math.max(count >> 5, 1):
            buffer[#index] = ~buffer[#index]
    }

    // Performs the bitwise AND operation between the elements of the current 'BitArray' and the corresponding elements in the specified array.
    // The current 'BitArray' will be modified to store the result of the bitwise AND operation.
    mtd andWith(other: Self)
    {
        Debug.assert(count == other.count)
        if !count:
            return
        for Math.max(count >> 5, 1):
            buffer[#index] &= other.buffer[#index]
    }

    // Performs the bitwise OR operation between the elements of the current 'BitArray' and the corresponding elements in the specified array.
    // The current BitArray will be modified to store the result of the bitwise OR operation.
    mtd orWith(other: Self)
    {
        Debug.assert(count == other.count)
        if !count:
            return
        for Math.max(count >> 5, 1):
            buffer[#index] |= other.buffer[#index]
    }

    // Performs the bitwise XOR operation between the elements of the current 'BitArray' and the corresponding elements in the specified array.
    // The current BitArray will be modified to store the result of the bitwise XOR operation.
    mtd xorWith(other: Self)
    {
        Debug.assert(count == other.count)
        if !count:
            return
        for Math.max(count >> 5, 1):
            buffer[#index] ^= other.buffer[#index]
    }
}
