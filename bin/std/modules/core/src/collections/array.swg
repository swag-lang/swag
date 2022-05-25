#global export
using Swag

struct(T) Array
{
    allocator:  IAllocator
    buffer:     *T
    count:      uint
    capacity:   uint
}

impl Array
{
    #[Inline]
    mtd opCount()->uint
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
        return @mkslice(buffer, count)
    }

    #[Inline]
    mtd opCast()->const [..] T
    {
        return @mkslice(buffer, count)
    }

    #[Inline]
    mtd opSlice(lower, upper: uint)->[..] T
    {
        Debug.assert(lower <= upper and lower < count and upper < count)
        return @mkslice(buffer + lower, (upper + 1) - lower)
    }

    #[Inline]
    mtd opIndex(index: uint)->*T
        #selectif Reflection.isStruct(T)
    {
        Debug.assert(index < count, "index out of range")
        return buffer + index
    }

    #[Inline]
    mtd opIndex(index: uint)->T
        #selectif !Reflection.isStruct(T)
    {
        Debug.assert(index < count, "index out of range")
        return buffer[index]
    }

    #[Inline]
    mtd opIndexAffect(index: uint, value: T)
        #selectif Reflection.canCopy(T)
    {
        Debug.assert(index < count, "index out of range")
        buffer[index] = value
    }

    mtd(op: string) opIndexAssign(index: uint, value: T)
    {
        Debug.assert(index < count, "index out of range")
        #ast "buffer[index] " ++ op ++ " value"
    }

    // Initializes an Array that contains values copied from the specified array
    mtd opAffect(arr: const [..] T)
        #selectif Reflection.canCopy(T)
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

    // Vist every elements of the array
    #[Macro]
    func(ptr: bool) opVisit(using const self, stmt: code)
    {
        if !count return

        // Little trick to avoid labels. That way a 'continue' in the user code will work
        // correctly, that's why we increment 'scan' before the use code, and take -1 to start.
        scan := buffer - 1

        loop count
        {
            scan += 1
            #macro
            {
                #if `ptr
                    var @alias0 = `scan
                #elif Reflection.isStruct(T)
                    var @alias0 = cast(const *T) `scan
                #else
                    var @alias0 = `scan[0]
                var @alias1 = @index
                #mixin `stmt
            }
        }
    }

    mtd free()
    {
        if !buffer
            return

        // Need to drop every structs
        #if Reflection.hasDrop(T)
        {
            loop count
                @drop(buffer + @index)
        }

        Memory.free(buffer, capacity * @sizeof(T), allocator)
    }

    mtd realloc(newCapacity: uint)
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

        newBuffer := cast(*T) Memory.realloc(buffer, newCapacity * @sizeof(T), capacity * @sizeof(T), allocator)

        // Need to call opPostMove/opPostCopy on every existing element if the buffer has not been reallocated
        // in place
        if buffer and newBuffer != buffer
        {
            #if Reflection.hasPostMove(T)
            {
                @postmove(newBuffer, count)
            }
            #elif Reflection.canCopy(T)
            {
                @postcopy(newBuffer, count)
                @drop(buffer, count)
            }
        }

        buffer = newBuffer
        capacity = newCapacity
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
    func createBuffer(size: uint)->Array'u8
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
    mtd grow(newCount : uint)
    {
        if newCount <= capacity
            return
        newCapacity := Math.max(capacity * 2, newCount)
        reserve(newCapacity)
    }

    // Reserve room for 'newCapacity' elements without changing the array count
    // Number of valid elements does not change
    mtd reserve(newCapacity: uint)
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
    mtd resize(newCount : uint)
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
        #selectif Reflection.canCopy(T)
    {
        if contains(value)
            return
        grow(count + 1)
        buffer[count] =,nodrop value
        count += 1
    }

    // Add a copy of one element at the end of the array
    mtd add(value: T)
        #selectif Reflection.canCopy(T)
    {
        grow(count + 1)
        buffer[count] =,nodrop value
        count += 1
    }

    // Add any value, as long as the type T accepts an opAffect of type G
    // or an implicit cast
    mtd(G) addAny(value: G)
        #selectif Reflection.canCopy(G)
    {
        #if @typeof(G) != @typeof(T)
        {
            var x: T = value
            add(x)
        }
        #else
        {
            add(value)
        }
    }

    // Reserve room at the end of the array for 'num' elements, but does not
    // initialize them. Returns the address of the first element
    mtd emplaceAddress(num: u32)->*T
    {
        grow(count + num)
        count += num
        return buffer + count - num
    }

    // Move one element at the end of the array
    mtd emplace(value: *T)
    {
        grow(count + 1)
        buffer[count] =,nodrop,move dref value
        count += 1
    }

    // Insert a value at the given index. If 'index' is equal to 'count', then
    // the value is added at the end of the array.
    // Order is preserved.
    mtd insertAt(index: uint, value: T)
        #selectif Reflection.canCopy(T)
    {
        Debug.assert(index <= count, "index out of range")
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

    // Insert some values at the given index. If 'index' is equal to 'count', then
    // the values are added at the end of the array.
    // Order is preserved.
    mtd insertAt(index: uint, values: const [..] T)
        #selectif Reflection.canCopy(T)
    {
        sliceCount := @countof(values)
        if !sliceCount
            return

        Debug.assert(index <= count, "index out of range")
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

    // Move a value at the given index. If 'index' is equal to 'count', then
    // the value is added at the end of the array.
    // Order is preserved.
    mtd emplaceAt(index: uint, value: *T)
    {
        Debug.assert(index <= count, "index out of range")
        if index == count
            return emplace(value)
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

        buffer[index] =,nodrop,move dref value
    }

    // Move some values at the given index. If 'index' is equal to 'count', then
    // the values are moved at the end of the array.
    // Order is preserved.
    mtd emplaceAt(index: uint, values: [..] T)
        #selectif Reflection.canCopy(T)
    {
        sliceCount := @countof(values)
        if !sliceCount
            return

        Debug.assert(index <= count, "index out of range")
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
        #selectif Reflection.canCopy(T)
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

    // Remove 'num' elements starting at 'index'
    mtd remove(index, num: uint)
    {
        if !num return
        Debug.assert(index + num <= count, "index out of range")

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
    mtd removeAt(index: uint)
    {
        Debug.assert(index < count, "index out of range")
        count -= 1
        if index == count
            @drop(buffer + count)
        else
            buffer[index] =,moveraw buffer[count]
    }

    // Remove 'numValues' elements at 'index' by shifting all others.
    // Order is preserved
    mtd removeAtOrdered(index: uint, numValues: uint = 1)
    {
        if !numValues
            return
        Debug.assert(index + numValues <= count, "index out of range")
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
    mtd remove(value: T)
    {
        (idx, found) := Slice.findLinear(@mkslice(buffer, count), value)
        if !found return
        removeAt(idx)
    }

    // Remove the given 'value'
    // If not found, does nothing
    mtd removeOrdered(value: T)
    {
        (idx, found) := Slice.findLinear(@mkslice(buffer, count), value)
        if !found return
        removeAtOrdered(idx)
    }

    // Returns true if the given 'value' is in the array
    mtd contains(value: T)->bool
    {
        res := Slice.findLinear(@mkslice(buffer, count), value)
        return res.success
    }

    // Sort array
    mtd sort()
    {
        Slice.sort(@mkslice(buffer, count), @(x, y: T)->s32 => x <=> y)
    }

    // Sort array in reverse order (from biggest to lowest value)
    mtd sortReverse()
    {
        Slice.sort(@mkslice(buffer, count), @(x, y: T)->s32 => y <=> x)
    }

    // Sort array
    mtd sort(cb: closure(T, T)->s32)
    {
        Debug.assert(cb != null)
        Slice.sort(@mkslice(buffer, count), cb)
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
        var result: retval = ?
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