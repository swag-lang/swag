#global export
using Swag

// This is a generic dynamic array.
struct(T) Array
{
    allocator:  IAllocator  // Associated allocator
    buffer:     ^T          // Memory block of all datas
    count:      u64         // Number of valid datas
    capacity:   u64         // Number of elements that can be stored in the buffer
}

impl Array
{
    #[Inline]
    mtd opCount()->u64
    {
        return count
    }

    #[Inline]
    mtd opData()->const *T
    {
        return buffer
    }

    mtd opDrop()
    {
        free()
    }

    mtd opPostCopy()
    {
        if buffer == null do
            return

        if @getcontext().flags & .PostCopyResetAllocator do
            allocator = null

        let oldBuffer = buffer
        buffer = null
        realloc(count)

        #if Reflection.hasPostCopy(T)
        {
            loop count do
                buffer[#index] =,nodrop oldBuffer[#index]
        }
        #else
        {
            Memory.copy(buffer, oldBuffer, @sizeof(T) * count)
        }
    }

    #[Inline, Overload]
    mtd opCast()->[..] T
    {
        return toSlice()
    }

    #[Inline, Overload]
    mtd const opCast()->const [..] T
    {
        return toSlice()
    }

    #[Inline]
    mtd opSlice(lower, upper: u64)->[..] T
    {
        Debug.safetyBoundCheck(lower <= upper, "lower bound greater than upper bound")
        Debug.safetyBoundCheck(lower < count, "lower bound out of range")
        Debug.safetyBoundCheck(upper < count, "upper bound out of range")
        return @mkslice(buffer + lower, (upper + 1) - lower)
    }

    #[Inline, Overload]
    mtd const opIndex(index: u64)->const &T
    {
        Debug.safetyBoundCheck(index < count, "index out of range")
        return buffer + index
    }

    #[Inline, Overload]
    mtd opIndex(index: u64)->&T
    {
        Debug.safetyBoundCheck(index < count, "index out of range")
        return buffer + index
    }

    #[Inline]
    mtd opIndexAffect(index: u64, value: T)
        #validif Reflection.canCopy(T)
    {
        Debug.safetyBoundCheck(index < count, "index out of range")
        buffer[index] = value
    }

    #[Inline]
    mtd(op: string) opIndexAssign(index: u64, value: T)
    {
        Debug.safetyBoundCheck(index < count, "index out of range")
        #ast "buffer[index] " ++ op ++ " value"
    }

    // Initializes an Array that contains values copied from the specified array
    mtd opAffect(arr: const [..] T)
        #validif Reflection.canCopy(T)
    {
        clear()
        var countof = @countof(arr)
        if !countof do return
        var dataof = @dataof(arr)

        grow(countof)
        #if Reflection.hasPostCopy(T)
        {
            loop i: countof do
                buffer[i] =,nodrop dataof[i]
        }
        #else
        {
            Memory.copy(buffer, dataof, @sizeof(T) * countof)
        }

        count += countof
    }

    // Visit every elements of the array.
    // Visiting by address and in reverse order is supported.
    #[Macro]
    func(ptr: bool, back: bool) opVisit(using const self, stmt: code)
    {
        if !count do
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work correctly
        #if back #do
            var scan = buffer + count
        #else #do
            var scan = buffer - 1

        loop count
        {
            #if back #do
                scan -= 1
            #else #do
                scan += 1

            #macro
            {
                #if #up ptr #do
                    let #alias0 = cast(*T) #up scan
                #elif Reflection.isStruct(T) #do
                    let #alias0 = cast(&T) #up scan
                #else #do
                    let #alias0 = #up scan[0]
                let #alias1 = #index
                #mixin #up stmt
            }
        }
    }

    // Returns a reference to a value given coordinate.
    // This is to use the array as an array with 2 dimensions.
    #[Inline]
    mtd ref1(x: u64)->&T
    {
        Debug.safetyBoundCheck(x < count, "index out of range")
        return buffer + x
    }

    // Returns a reference to a value given 2 coordinates and a width.
    // This is to use the array as an array with 2 dimensions.
    #[Inline]
    mtd ref2(x, y, w: u64)->&T
    {
        Debug.safetyBoundCheck(x + y*w < count, "index out of range")
        return buffer + x + y*w
    }

    mtd realloc(newCapacity: u64)
    {
        if allocator == null do
            allocator = @getcontext().allocator

        if newCapacity == 0
        {
            Memory.free(buffer, capacity * @sizeof(T), allocator)
            buffer = null
            capacity = 0
            return
        }

        let newBuffer = cast(^T) Memory.realloc(buffer, newCapacity * @sizeof(T), capacity * @sizeof(T), allocator)

        // Need to call opPostMove on every existing element if the buffer has not been reallocated
        // in place
        #if Reflection.hasPostMove(T)
        {
            if buffer and newBuffer != buffer
            {
                @postmove(newBuffer, count)
            }
        }

        buffer = newBuffer
        capacity = newCapacity
    }

    // Free the array content
    mtd free()
    {
        if !buffer do
            return

        // Need to drop every structs
        #if Reflection.hasDrop(T)
        {
            @drop(buffer, count)
        }

        Memory.free(buffer, capacity * @sizeof(T), allocator)
        buffer = null
        count, capacity  = 0
    }

    // Create a temporary array
    #[Inline]
    func(TA) createTemp()->Array'TA
    {
        var result: retval
        result.allocator = @getcontext().tempAllocator
        return result
    }

    // Create a working buffer
    #[NotGeneric]
    func createBuffer(size: u64)->Array'u8
    {
        var result: retval
        result.resize(size)
        return result
    }

    // Returns true if the array is empty
    #[Inline]
    mtd const isEmpty() => count == 0

    // Returns a slice
    #[Inline, Overload]
    mtd toSlice()->[..] T
    {
        return @mkslice(buffer, count)
    }

    // Returns a slice
    #[Inline, Overload]
    mtd const toSlice()->const [..] T
    {
        return @mkslice(buffer, count)
    }

    // Ensure the Array is big enough to store at least 'newCount' elements.
    // The number of valid elements does not change.
    mtd grow(newCount: u64)
    {
        if newCount <= capacity do
            return
        let newCapacity = Math.max(capacity * 2, newCount)
        reserve(newCapacity)
    }

    // Reserve room for 'newCapacity' elements without changing the array count.
    // The number of valid elements does not change.
    mtd reserve(newCapacity: u64)
    {
        if !newCapacity
        {
            free()
            buffer = null
            count, capacity = 0
            return
        }

        if newCapacity <= capacity do
            return

        realloc(newCapacity)
    }

    // Change the number of valid elements in the array
    mtd resize(newCount: u64)
    {
        if newCount == count do
            return
        if newCount > count
        {
            grow(newCount)
            @init(buffer + count, newCount - count)
            count = newCount
        }
        else
        {
            @drop(buffer + newCount, count - newCount)
            count = newCount
        }
    }

    // Set the number of elements to 0
    mtd clear()
    {
        #if Reflection.hasDrop(T)
        {
            loop count do
                @drop(buffer + #index)
        }

        count = 0
    }

    // Add a copy of one element at the end of the array
    mtd addOnce(value: T)
        #validif Reflection.canCopy(T)
    {
        if contains(value) do
            return
        grow(count + 1)
        buffer[count] =,nodrop value
        count += 1
    }

    // Add a copy of one element at the end of the array
    #[Overload]
    mtd add(value: T)
        #validif Reflection.canCopy(T)
    {
        grow(count + 1)
        buffer[count] =,nodrop value
        count += 1
    }

    // Move one element at the end of the array
    #[Overload]
    mtd add(value: &&T)
    {
        grow(count + 1)
        buffer[count] =,nodrop,move value
        count += 1
    }

    // Reserve room at the end of the array for 'num' elements, but does not
    // initialize them. Returns the address of the first element
    mtd emplaceAddress(num: u32 = 1)->^T
    {
        grow(count + num)
        count += num
        return buffer + count - num
    }

    // Reserve room at the end of the array for 'num' elements.
    // Returns the address of the first element
    mtd emplaceInitAddress(num: u32 = 1)->^T
    {
        grow(count + num)
        count += num
        var res = buffer + count - num
        @init(res, num)
        return res
    }

    // Insert a value at the given index. If 'index' is equal to 'count', then
    // the value is added at the end of the array.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: T)
        #validif Reflection.canCopy(T)
    {
        Debug.safetyBoundCheck(index <= count, "index out of range")
        if index == count do
            return add(value)
        grow(count + 1)
        count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = count - 1; idx > index; idx -= 1 do
                buffer[idx] =,nodrop,moveraw buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(buffer + index + 1, buffer + index, @sizeof(T) * (count - 1 - index))
        }

        buffer[index] =,nodrop value
    }

    // Move a value at the given index. If 'index' is equal to 'count', then
    // the value is added at the end of the array.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: &&T)
    {
        Debug.safetyBoundCheck(index <= count, "index out of range")
        if index == count do
            return add(moveref value)
        grow(count + 1)
        count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = count - 1; idx > index; idx -= 1 do
                buffer[idx] =,nodrop,moveraw buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(buffer + index + 1, buffer + index, @sizeof(T) * (count - 1 - index))
        }

        buffer[index] =,nodrop,move value
    }

    // Insert some values at the given index. If 'index' is equal to 'count', then
    // the values are added at the end of the array.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, values: const [..] T)
        #validif Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return

        Debug.safetyBoundCheck(index <= count, "index out of range")
        if index == count do
            return add(values)
        grow(count + sliceCount)
        count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = count - 1; idx >= index + sliceCount; idx -= 1 do
                buffer[idx] =,nodrop,moveraw buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(buffer + index + sliceCount, buffer + index, @sizeof(T) * (count - sliceCount - index))
        }

        #if Reflection.hasPostCopy(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1 do
                buffer[index + idx] =,nodrop values[idx]
        }
        #else
        {
            Memory.copy(buffer + index, @dataof(values), @sizeof(T) * sliceCount)
        }
    }

    // Move some values at the given index. If 'index' is equal to 'count', then
    // the values are moved at the end of the array.
    // Order is preserved.
    mtd emplaceAt(index: u64, values: [..] T)
        #validif Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return

        Debug.safetyBoundCheck(index <= count, "index out of range")
        if index == count do
            return add(values)
        grow(count + sliceCount)
        count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = count - 1; idx >= index + sliceCount; idx -= 1 do
                buffer[idx] =,nodrop,moveraw buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(buffer + index + @countof(values), buffer + index, @sizeof(T) * (count - sliceCount - index))
        }

        #if Reflection.hasPostMove(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1 do
                buffer[index + idx] =,nodrop,move values[idx]
        }
        #else
        {
            Memory.copy(buffer + index, @dataof(values), @sizeof(T) * sliceCount)
        }
    }

    // Append a slice to the end of this instance
    #[Overload]
    mtd add(values: const [..] T)
        #validif Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return

        // Do not add myself if there's not enough room (because we will reallocate)
        if count + sliceCount > capacity do
            Debug.assert(@dataof(values) < buffer or @dataof(values) > buffer + capacity)

        grow(count + sliceCount)
        var oldCount = count
        count += sliceCount

        #if Reflection.hasPostCopy(T)
        {
            var destPtr = buffer + oldCount
            loop sliceCount
            {
                dref destPtr =,nodrop values[#index]
                destPtr += 1
            }
        }
        #else
        {
            Memory.copy(buffer + oldCount, @dataof(values), sliceCount * @sizeof(T))
        }
    }

    // Swap two elements
    mtd swap(idx0, idx1: u64)
    {
        if idx0 == idx1 do
            return
        Debug.safetyBoundCheck(idx0 < count, "index out of range")
        Debug.safetyBoundCheck(idx1 < count, "index out of range")
        swap(buffer + idx0, buffer + idx1)
    }

    // Remove 'num' elements starting at 'index'
    #[Overload]
    mtd remove(index, num: u64)
    {
        if !num do
            return
        Debug.safetyBoundCheck(index + num <= count, "index out of range")

        #if !Reflection.hasPostMove(T) and !Reflection.hasDrop(T)
        {
            Memory.copyOver(buffer + index, buffer + index + num, (count - (index + num)) * @sizeof(T))
        }
        #else
        {
            loop i: count - num
            {
                buffer[index + i] =,moveraw buffer[index + num + i]
            }
        }

        count -= num
    }

    // Remove the given 'value'
    // If not found, does nothing
    #[Overload]
    mtd(V) remove(value: V)
    {
        let idx = Slice.findLinear(toSlice(), value)
        if idx == Swag.U64.Max do
            return
        removeAt(idx)
    }

    // Remove an element at 'index' by replacing it with the last element.
    // Order is **not preserved**
    mtd removeAt(index: u64)
    {
        Debug.safetyBoundCheck(index < count, "index out of range")
        count -= 1
        if index == count do
            @drop(buffer + count)
        else do
            buffer[index] =,moveraw buffer[count]
    }

    // Remove 'numValues' elements at 'index' by shifting all others.
    // Order is preserved
    mtd removeAtOrdered(index: u64, numValues: u64 = 1)
    {
        if !numValues do
            return
        Debug.safetyBoundCheck(index + numValues <= count, "index out of range")
        count -= numValues

        #if Reflection.hasDrop(T)
        {
            loop idx: numValues do
                @drop(buffer + index + idx)
        }

        if index >= count do
            return

        #if Reflection.hasPostMove(T)
        {
            var idx = index
            loop count - index
            {
                buffer[idx] =,nodrop,moveraw buffer[idx + numValues]
                idx += 1
            }
        }
        #else
        {
            Memory.copyOver(buffer + index, buffer + index + numValues, @sizeof(T) * (count - index))
        }
    }

    // Remove the given 'value'
    // If not found, does nothing
    mtd(V) removeOrdered(value: V)
    {
        let idx = Slice.findLinear(toSlice(), value)
        if idx == Swag.U64.Max do
            return
        removeAtOrdered(idx)
    }

    // Returns true if the given 'value' is in the array
    mtd(V) contains(value: V)->bool
    {
        return Slice.findLinear(toSlice(), value) != Swag.U64.Max
    }

    // Sort array
    #[Overload]
    mtd sort()
    {
        Slice.sort(toSlice(), func(x, y: T)->s32 => x <=> y)
    }

    // Sort array in reverse order (from biggest to lowest value)
    mtd sortReverse()
    {
        Slice.sort(toSlice(), func(x, y: T)->s32 => y <=> x)
    }

    // Sort array
    #[Overload]
    mtd sort(cb: closure(T, T)->s32)
    {
        Debug.assert(cb != null)
        Slice.sort(toSlice(), cb)
    }

    // Returns a copy of the first element
    mtd front()->T
    {
        Debug.assert(count > 0, "array is empty")
        let result: retval = buffer[0]
        return result
    }

    // Returns the address of the first element
    mtd frontPtr()->*T
    {
        Debug.assert(count > 0, "array is empty")
        return buffer
    }

    // Returns a copy of the last element
    mtd back()->T
    {
        Debug.assert(count > 0, "array is empty")
        let result: retval = buffer[count - 1]
        return result
    }

    // Returns the address of the last element
    mtd backPtr()->*T
    {
        Debug.assert(count > 0, "array is empty")
        return buffer + count - 1
    }

    // Returns a copy of the last element, and remove it from the array
    mtd popBack()->T
    {
        Debug.assert(count > 0, "array is empty")
        var result: retval = undefined
        result =,nodrop,moveraw buffer[count - 1]
        count -= 1
        return result
    }

    // Remove the last element
    mtd removeBack()
    {
        Debug.assert(count > 0, "array is empty")
        @drop(buffer + count - 1)
        count -= 1
    }
}