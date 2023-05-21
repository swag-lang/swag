#global export
using Swag

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
        if buffer == null
            return

        oldBuffer := buffer
        buffer = null
        realloc(count)

        #if Reflection.hasPostCopy(T)
        {
            loop count
                buffer[@index] =,nodrop oldBuffer[@index]
        }
        #else
        {
            Memory.copy(buffer, oldBuffer, @sizeof(T) * count)
        }
    }

    #[Inline]
    mtd opCast()->[..] T
    {
        return toSlice()
    }

    #[Inline]
    mtd opCast()->const [..] T
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

    #[Inline]
    mtdc opIndex(index: u64)->ref T
        #validif Reflection.isStruct(T)
    {
        Debug.safetyBoundCheck(index < count, "index out of range")
        return buffer + index
    }

    #[Inline]
    mtdc opIndex(index: u64)->T
        #validif !Reflection.isStruct(T)
    {
        Debug.safetyBoundCheck(index < count, "index out of range")
        return buffer[index]
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
        countof := @countof(arr)
        if !countof return
        dataof := @dataof(arr)

        grow(countof)
        #if Reflection.hasPostCopy(T)
        {
            loop i: countof
                buffer[i] =,nodrop dataof[i]
        }
        #else
        {
            Memory.copy(buffer, dataof, @sizeof(T) * countof)
        }

        count += countof
    }

    // Visit every elements of the array
    #[Macro]
    func(ptr: bool) opVisit(using const self, stmt: code)
    {
        if !count
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work
        // correctly, that's why we increment 'scan' before the use code, and take -1 to start.
        scan := buffer - 1

        loop count
        {
            scan += 1
            #macro
            {
                #if #up ptr
                    var @alias0 = cast(*T) #up scan
                #elif Reflection.isStruct(T)
                    var @alias0 = cast(ref T) #up scan
                #else
                    var @alias0 = #up scan[0]
                var @alias1 = @index
                #mixin #up stmt
            }
        }
    }

    // Visit every elements of the array from end to start
    #[Macro]
    func(ptr: bool) opVisitBack(using const self, stmt: code)
    {
        if !count return

        // Little trick to avoid labels. That way a 'continue' in the user code will work
        // correctly, that's why we decrement 'scan' before the use code.
        scan := buffer + count

        loop count
        {
            scan -= 1
            #macro
            {
                #if #up ptr
                    var @alias0 = #up scan
                #elif Reflection.isStruct(T)
                    var @alias0 = cast(ref T) #up scan
                #else
                    var @alias0 = #up scan[0]
                var @alias1 = @index
                #mixin #up stmt
            }
        }
    }

    mtd realloc(newCapacity: u64)
    {
        if allocator == null
            allocator = @getcontext().allocator

        if newCapacity == 0
        {
            Memory.free(buffer, capacity * @sizeof(T), allocator)
            buffer = null
            capacity = 0
            return
        }

        newBuffer := cast(^T) Memory.realloc(buffer, newCapacity * @sizeof(T), capacity * @sizeof(T), allocator)

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
        if !buffer
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
    #[Swag.Inline]
    func(TA) createTemp()->Array'TA
    {
        var result: retval
        result.allocator = @getcontext().tempAllocator
        return result
    }

    // Create a working buffer
    #[Swag.NotGeneric]
    func createBuffer(size: u64)->Array'u8
    {
        var result: retval
        result.resize(size)
        return result
    }

    // Returns true if the array is empty
    #[Inline]
    mtdc isEmpty() => count == 0

    // Returns a slice
    #[Inline]
    mtd toSlice()->[..] T
    {
        return @mkslice(buffer, count)
    }

    // Ensure the Array is big enough to store at least 'newCount' elements
    // Number of valid elements does not change
    mtd grow(newCount : u64)
    {
        if newCount <= capacity
            return
        newCapacity := Math.max(capacity * 2, newCount)
        reserve(newCapacity)
    }

    // Reserve room for 'newCapacity' elements without changing the array count
    // Number of valid elements does not change
    mtd reserve(newCapacity: u64)
    {
        if !newCapacity
        {
            free()
            buffer = null
            count, capacity = 0
            return
        }

        if newCapacity <= capacity
            return

        realloc(newCapacity)
    }

    // Change the number of valid elements in the array
    mtd resize(newCount : u64)
    {
        if newCount == count
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
            loop count
                @drop(buffer + @index)
        }

        count = 0
    }

    // Add a copy of one element at the end of the array
    mtd addOnce(value: T)
        #validif Reflection.canCopy(T)
    {
        if contains(value)
            return
        grow(count + 1)
        buffer[count] =,nodrop value
        count += 1
    }

    // Add a copy of one element at the end of the array
    mtd add(value: T)
        #validif Reflection.canCopy(T)
    {
        grow(count + 1)
        buffer[count] =,nodrop value
        count += 1
    }

    // Move one element at the end of the array
    mtd add(value: moveref T)
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
        res := buffer + count - num
        @init(res, num)
        return res
    }

    // Insert a value at the given index. If 'index' is equal to 'count', then
    // the value is added at the end of the array.
    // Order is preserved.
    mtd insertAt(index: u64, value: T)
        #validif Reflection.canCopy(T)
    {
        Debug.safetyBoundCheck(index <= count, "index out of range")
        if index == count
            return add(value)
        grow(count + 1)
        count += 1

        #if Reflection.hasPostMove(T)
        {
            for idx := count - 1; idx > index; idx -= 1
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
    mtd insertAt(index: u64, value: moveref T)
    {
        Debug.safetyBoundCheck(index <= count, "index out of range")
        if index == count
            return add(moveref value)
        grow(count + 1)
        count += 1

        #if Reflection.hasPostMove(T)
        {
            for idx := count - 1; idx > index; idx -= 1
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
    mtd insertAt(index: u64, values: const [..] T)
        #validif Reflection.canCopy(T)
    {
        sliceCount := @countof(values)
        if !sliceCount
            return

        Debug.safetyBoundCheck(index <= count, "index out of range")
        if index == count
            return add(values)
        grow(count + sliceCount)
        count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for idx := count - 1; idx >= index + sliceCount; idx -= 1
                buffer[idx] =,nodrop,moveraw buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(buffer + index + sliceCount, buffer + index, @sizeof(T) * (count - sliceCount - index))
        }

        #if Reflection.hasPostCopy(T)
        {
            for idx := 0; idx < sliceCount; idx += 1
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
        sliceCount := @countof(values)
        if !sliceCount
            return

        Debug.safetyBoundCheck(index <= count, "index out of range")
        if index == count
            return add(values)
        grow(count + sliceCount)
        count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for idx := count - 1; idx >= index + sliceCount; idx -= 1
                buffer[idx] =,nodrop,moveraw buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(buffer + index + @countof(values), buffer + index, @sizeof(T) * (count - sliceCount - index))
        }

        #if Reflection.hasPostMove(T)
        {
            for idx := 0; idx < sliceCount; idx += 1
                buffer[index + idx] =,nodrop,move values[idx]
        }
        #else
        {
            Memory.copy(buffer + index, @dataof(values), @sizeof(T) * sliceCount)
        }
    }

    // Append a slice to the end of this instance
    mtd add(values: const [..] T)
        #validif Reflection.canCopy(T)
    {
        sliceCount := @countof(values)
        if !sliceCount
            return

        // Do not add myself if there's not enough room (because we will reallocate)
        if count + sliceCount > capacity
            Debug.assert(@dataof(values) < buffer or @dataof(values) > buffer + capacity)

        grow(count + sliceCount)
        oldCount := count
        count += sliceCount

        #if Reflection.hasPostCopy(T)
        {
            destPtr := buffer + oldCount
            loop sliceCount
            {
                dref destPtr =,nodrop values[@index]
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
        if idx0 == idx1
            return
        Debug.safetyBoundCheck(idx0 < count, "index out of range")
        Debug.safetyBoundCheck(idx1 < count, "index out of range")
        swap(buffer + idx0, buffer + idx1)
    }

    // Remove 'num' elements starting at 'index'
    mtd remove(index, num: u64)
    {
        if !num return
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

    // Remove an element at 'index' by replacing it with the last element.
    // Order is not preserved
    mtd removeAt(index: u64)
    {
        Debug.safetyBoundCheck(index < count, "index out of range")
        count -= 1
        if index == count
            @drop(buffer + count)
        else
            buffer[index] =,moveraw buffer[count]
    }

    // Remove 'numValues' elements at 'index' by shifting all others.
    // Order is preserved
    mtd removeAtOrdered(index: u64, numValues: u64 = 1)
    {
        if !numValues
            return
        Debug.safetyBoundCheck(index + numValues <= count, "index out of range")
        count -= numValues

        #if Reflection.hasDrop(T)
        {
            loop idx: numValues
                @drop(buffer + index + idx)
        }

        if index >= count
            return

        #if Reflection.hasPostMove(T)
        {
            idx := index
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
    mtd(V) remove(value: V)
    {
        (idx, found) := Slice.findLinear(toSlice(), value)
        if !found return
        removeAt(idx)
    }

    // Remove the given 'value'
    // If not found, does nothing
    mtd(V) removeOrdered(value: V)
    {
        (idx, found) := Slice.findLinear(toSlice(), value)
        if !found return
        removeAtOrdered(idx)
    }

    // Returns true if the given 'value' is in the array
    mtd(V) contains(value: V)->bool
    {
        res := Slice.findLinear(toSlice(), value)
        return res.success
    }

    // Sort array
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
    mtd sort(cb: closure(T, T)->s32)
    {
        Debug.assert(cb != null)
        Slice.sort(toSlice(), cb)
    }

    // Returns a copy of the first element
    mtd front()->T
    {
        Debug.assert(count > 0, "array is empty")
        var result: retval = buffer[0]
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
        var result: retval = buffer[count - 1]
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