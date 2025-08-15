#global marked
using Swag

// Manages a compact array of bit values.
// Bits are represented as booleans, where 'true' indicates the bit is on (1)
// and 'false' indicates the bit is off (0). This structure is memory-efficient
// for storing a large number of boolean flags.
public struct BitArray
{
    buffer:        ^u32           // Internal buffer storing the bits packed into 32-bit integers.
    allocator:     IAllocator     // The allocator used for memory operations.
    count:         u64            // The number of bits in the array.
    capacity:      u64            // The total allocated capacity in bytes.
}

impl BitArray
{
    // Frees the memory allocated by the bit array.
    // Does nothing if the buffer is not allocated.
    mtd free()
    {
        if !self.buffer:
            return
        Memory.free(self.buffer, self.capacity, self.allocator)
    }

    // Reallocates the internal buffer to a new capacity.
    // This is a low-level memory operation.
    mtd realloc(newCapacity: u64)
    {
        if self.allocator == null:
            self.allocator = @getcontext().allocator
        self.buffer = cast() Memory.realloc(self.buffer, newCapacity, self.capacity, self.allocator)
    }
}

public impl BitArray
{
    // Returns the number of bits in the array.
    mtd const opCount()->u64
    {
        return self.count
    }

    // Ensures the allocated memory is freed when the array goes out of scope.
    mtd opDrop()
    {
        self.free()
    }

    // Handles the deep copy of the array when it is copied.
    // A new buffer is allocated and the content is copied over.
    mtd opPostCopy()
    {
        if self.buffer
        {
            let otherBuffer = self.buffer
            self.buffer = null
            self.reserve(self.count)
            Memory.copy(self.buffer, otherBuffer, self.capacity)
        }
    }

    // Returns the boolean value of the bit at a specific position.
    mtd opIndex(index: u64)->bool
    {
        return self.get(index)
    }

    // Sets the value of the bit at a specific position.
    mtd opIndexAffect(index: u64, value: bool)
    {
        self.set(index, value)
    }

    // Replaces the content of the bit array with values from a slice of booleans.
    mtd opAffect(what: const [..] bool)
    {
        let len = @countof(what)
        self.reserve(len)
        self.buffer[len >> 5] = 0
        foreach value, index in what:
            self.set(index, value)
    }

    // Compares this bit array with another one for equality.
    // Returns 'true' if both arrays have the same bit count and all bits are identical.
    mtd opEquals(other: BitArray)->bool
    {
        if self.count != other.count:
            return false

        for Math.max(self.count >> 5, 1)
        {
            if self.buffer[@index] != other.buffer[@index]:
                return false
        }

        return true
    }

    // Provides a way to iterate over each bit in the array.
    // This is a macro that accepts a code block to execute for each bit.
    // Aliases: #alias0 for the boolean value, #alias1 for the index.
    #[Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        #if ptr:
            #error("visiting by pointer is not supported for BitArray")
        #if back:
            #error("visiting in reverse order is not supported for BitArray")

        if !self.count:
            return
        let remainingBits = self.count
        var countBits     = 0'u64

        #scope(Up)
        for idx in Math.max(self.count >> 5, 1)
        {
            var shift: u32 = 1
            for Math.min(remainingBits, 32)
            {
                #scope(Down)
                #macro
                {
                    let #alias0 = cast(bool) (#up self.buffer[#up idx] & #up shift)
                    let #alias1 = #up countBits
                    #inject(#up stmt, break = break to Up, continue = break to Down)
                }

                countBits += 1
                shift <<= 1
            }
        }
    }

    // Reserves capacity for a given number of bits.
    // The allocated memory is not initialized.
    #[Swag.Overload]
    mtd reserve(maxBits: u64)
    {
        if !maxBits
        {
            self.free()
            self.buffer = null
            self.count, self.capacity = 0
            return
        }

        self.count    = maxBits
        self.capacity = maxBits >> 5
        if self.capacity << 5 < maxBits:
            self.capacity += 1

        self.capacity *= 4
        self.realloc(self.capacity)
    }

    // Reserves capacity for a given number of bits and initializes them to a specific value.
    #[Swag.Overload]
    mtd reserve(maxBits: u64, initialValue: bool)
    {
        self.reserve(maxBits)
        Memory.set(self.buffer, initialValue ? 0xFF : 0x00, self.capacity)
    }

    // Sets all bits in the array to the specified boolean value.
    mtd setAll(value: bool)
    {
        Memory.set(self.buffer, value ? 0xFF : 0x00, self.capacity)
    }

    // Sets the bit at a specific position to the specified value.
    mtd set(index: u64, value: bool)
    {
        Debug.assert(index < self.count)
        let val   = index >> 5
        let bit   = index - (val << 5)
        let shift = 1'u32 << cast(u32) bit
        if value:
            self.buffer[val] |= shift
        else:
            self.buffer[val] &= ~shift
    }

    // Returns the boolean value of the bit at a specific position.
    mtd const get(index: u64)->bool
    {
        Debug.assert(index < self.count)
        let val   = index >> 5
        let bit   = index - (val << 5)
        let shift = 1'u32 << cast(u32) bit
        return cast(bool) (self.buffer[val] & shift)
    }

    // Inverts the value of the bit at a specific position.
    // Changes a 'true' bit to 'false' and a 'false' bit to 'true'.
    #[Swag.Overload]
    mtd invert(index: u64)
    {
        Debug.assert(index < self.count)
        let val   = index >> 5
        let bit   = index - (val << 5)
        let shift = 1'u32 << cast(u32) bit
        if self.buffer[val] & shift:
            self.buffer[val] ^= shift
        else:
            self.buffer[val] |= shift
    }

    // Inverts all bit values in the array.
    // Bits set to 'true' are changed to 'false', and vice versa.
    #[Swag.Overload]
    mtd invert()
    {
        if !self.count:
            return
        for Math.max(self.count >> 5, 1):
            self.buffer[@index] = ~self.buffer[@index]
    }

    // Performs a bitwise AND operation between this array and another.
    // The current array is modified to store the result. Asserts that arrays are of the same size.
    mtd andWith(other: BitArray)
    {
        Debug.assert(self.count == other.count)
        if !self.count:
            return
        for Math.max(self.count >> 5, 1):
            self.buffer[@index] &= other.buffer[@index]
    }

    // Performs a bitwise OR operation between this array and another.
    // The current array is modified to store the result. Asserts that arrays are of the same size.
    mtd orWith(other: BitArray)
    {
        Debug.assert(self.count == other.count)
        if !self.count:
            return
        for Math.max(self.count >> 5, 1):
            self.buffer[@index] |= other.buffer[@index]
    }

    // Performs a bitwise XOR operation between this array and another.
    // The current array is modified to store the result. Asserts that arrays are of the same size.
    mtd xorWith(other: BitArray)
    {
        Debug.assert(self.count == other.count)
        if !self.count:
            return
        for Math.max(self.count >> 5, 1):
            self.buffer[@index] ^= other.buffer[@index]
    }
}
