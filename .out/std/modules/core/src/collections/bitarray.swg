using Swag

// Manages a compact array of bit values, which are represented as booleans, where 'true' indicates that
// the bit is on (1) and 'false' indicates the bit is off (0)
public struct BitArray
{
    buffer:     *u32
    allocator:  IAllocator
    count:      uint
    capacity:   uint
}

impl BitArray
{
    func free(using self)
    {
        if !buffer
            return
        Memory.free(buffer, capacity, allocator)
    }

    func realloc(using self, newCapacity: uint)
    {
        if allocator == null
            allocator = @getcontext().allocator
        buffer = acast Memory.realloc(buffer, newCapacity, capacity, allocator)
    }
}

public impl BitArray
{
    func opCount(using self)->uint
    {
        return count
    }

    func opDrop(using self)
    {
        free()
    }

    func opPostCopy(using self)
    {
        if(buffer)
        {
            otherBuffer := buffer
            buffer = null
            reserve(count)
            Memory.copy(buffer, otherBuffer, capacity)
        }
    }

    // Gets the value of the bit at a specific position
    func opIndex(using self, index: uint)->bool
    {
        return get(index)
    }

    // Sets the value of the bit at a specific position
    func opIndexAffect(using self, index: uint, value: bool)
    {
        set(index, value)
    }

    // Initializes a BitArray that contains bit values copied from the specified array of booleans
    func opAffect(using self, what: const [..] bool)
    {
        len := @countof(what)
        reserve(len)
        buffer[len >>,safe 5] = 0
        visit value, index: what
            set(index, value)
    }

    // Compares two arrays, and returns 'true' if they are equal
    func opEquals(using self, other: Self)->bool
    {
        if count != other.count
            return false

        loop math.max(count >>,safe 5, 1)
        {
            if buffer[@index] != other.buffer[@index]
                return false
        }

        return true
    }

    // Visit all the booleans
    #[macro]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        #assert(ptr == false, "pointer visit not supported for BitArray")
        if !count return
        remainingBits := count
        countBits := 0'uint

        label up
        loop idx: math.max(count >>,safe 5, 1)
        {
            var shift: u32 = 1
            loop math.min(remainingBits, 32)
            {
                label down
                #macro
                {
                    var @alias0 = cast(bool) (`buffer[`idx] & `shift)
                    var @alias1 = `countBits
                    #mixin `stmt { break = break up; continue = break down; }
                }

                countBits += 1
                shift <<= 1
            }
        }
    }

    // Reserve the given amount of bits
    func reserve(using self, maxBits: uint)
    {
        if !maxBits
        {
            free()
            buffer = null
            count, capacity = 0
            return
        }

        count = maxBits
        capacity = maxBits >>,safe 5
        if capacity << 5 < maxBits
            capacity += 1

        capacity *= 4
        realloc(capacity)
    }

    // Reserve the given amount of bits and set an initial value to all bits
    func reserve(using self, maxBits: uint, initialValue: bool)
    {
        reserve(maxBits)
        Memory.set(buffer, initialValue ? 0xFF : 0x00, capacity)
    }

    // Sets all bits to the specified value
    func setAll(using self, value: bool)
    {
        Memory.set(buffer, value ? 0xFF : 0x00, capacity)
    }

    // Sets the bit at a specific position to the specified value
    func set(using self, index: uint, value: bool)
    {
        Debug.assert(index < count)
        val := index >>,safe 5
        bit := index - (val << 5)
        var shift = 1'u32 << cast(u32) bit
        if value
            buffer[val] |= shift
        else
            buffer[val] &= ~shift
    }

    // Gets the value of the bit at a specific position
    func get(using self, index: uint)->bool
    {
        Debug.assert(index < count)
        val := index >>,safe 5
        bit := index - (val << 5)
        var shift = 1'u32 << cast(u32) bit
        return cast(bool) (buffer[val] & shift)
    }

    // Inverts the value of the bit at a specific position
    func invert(using self, index: uint)
    {
        Debug.assert(index < count)
        val := index >>,safe 5
        bit := index - (val << 5)
        var shift = 1'u32 << cast(u32) bit
        if buffer[val] & shift
            buffer[val] ^= shift
        else
            buffer[val] |= shift
    }

    // Performs the bitwise AND operation between the elements of the current BitArray and the corresponding elements in the specified array.
    // The current BitArray will be modified to store the result of the bitwise AND operation.
    func and(using self, other: Self)
    {
        Debug.assert(count == other.count)
        if !count return
        loop math.max(count >>,safe 5, 1)
            buffer[@index] &= other.buffer[@index]
    }

    // Performs the bitwise OR operation between the elements of the current BitArray and the corresponding elements in the specified array.
    // The current BitArray will be modified to store the result of the bitwise OR operation.
    func or(using self, other: Self)
    {
        Debug.assert(count == other.count)
        if !count return
        loop math.max(count >>,safe 5, 1)
            buffer[@index] |= other.buffer[@index]
    }

    // Performs the bitwise XOR operation between the elements of the current BitArray and the corresponding elements in the specified array.
    // The current BitArray will be modified to store the result of the bitwise XOR operation.
    func xor(using self, other: Self)
    {
        Debug.assert(count == other.count)
        if !count return
        loop math.max(count >>,safe 5, 1)
            buffer[@index] ^= other.buffer[@index]
    }

    // Inverts all the bit values, so that elements set to 'true' are changed to `false`, and elements set to `false` are changed to 'true'.
    func not(using self)
    {
        if !count return
        loop math.max(count >>,safe 5, 1)
            buffer[@index] = ~buffer[@index]
    }
}
