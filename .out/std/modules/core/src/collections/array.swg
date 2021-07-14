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
    func opCount(using self)->uint
    {
        return count
    }

    func opData(using self)->const *T
    {
        return buffer
    }

    func opDrop(using self)
    {
        free()
    }

    func opPostCopy(using self)
    {
        if buffer == null
            return

        oldBuffer := buffer
        buffer = null
        realloc(count)

        #if Reflection.hasPostCopy(@typeof(T))
        {
            loop count
                buffer[@index] =,nodrop oldBuffer[@index]
        }
        #else
        {
            Memory.copy(buffer, oldBuffer, @sizeof(T) * count)
        }
    }

    func opCast(using self)->[..] T
    {
        return @mkslice(buffer, count)
    }

    func opCast(using self)->const [..] T
    {
        return @mkslice(buffer, count)
    }

    func opSlice(using self, lower, upper: uint)->[..] T
    {
        Debug.assert(lower <= upper and lower < count and upper < count)
        return @mkslice(buffer + lower, (upper + 1) - lower)
    }

    #[Swag.Inline]
    func opIndex(using self, index: uint)->*T
        #selectif Reflection.isStruct(T)
    {
        Debug.assert(index < count, "index out of range")
        return buffer + index
    }

    #[Swag.Inline]
    func opIndex(using self, index: uint)->T
        #selectif !Reflection.isStruct(T)
    {
        Debug.assert(index < count, "index out of range")
        return buffer[index]
    }

    func opIndexAffect(using self, index: uint, value: T)
        #selectif Reflection.canCopy(@typeof(T))
    {
        Debug.assert(index < count, "index out of range")
        buffer[index] = value
    }

    func(op: string) opIndexAssign(using self, index: uint, value: T)
    {
        Debug.assert(index < count, "index out of range")
        #ast "buffer[index] " ~op~ " value"
    }

    // Initializes an Array that contains values copied from the specified array
    func opAffect(using self, arr: const [..] T)
        #selectif Reflection.canCopy(@typeof(T))
    {
        clear()
        countof := @countof(arr)
        if !countof return
        dataof := @dataof(arr)

        grow(countof)
        #if Reflection.hasPostCopy(@typeof(T))
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
    #[macro]
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
                #else
                    var @alias0 = Reflection.isStruct(T) ? cast(const *T) `scan : `scan[0]
                var @alias1 = @index
                #mixin `stmt
            }
        }
    }

    method free()
    {
        if !buffer
            return

        // Need to drop every structs
        #if Reflection.hasDrop(@typeof(T))
        {
            loop count
                @drop(buffer + @index)
        }

        Memory.free(buffer, capacity * @sizeof(T), allocator)
    }

    method realloc(newCapacity: uint)
    {
        if allocator == null
            allocator = @getcontext().allocator

        newBuffer := cast(*T) Memory.realloc(buffer, newCapacity * @sizeof(T), capacity * @sizeof(T), allocator)

        // Need to call opPostMove/opPostCopy on every existing element if the buffer has not been reallocated
        // in place
        if buffer and newBuffer != buffer
        {
            @reloc(newBuffer, buffer, count)
            #if Reflection.hasPostMove(@typeof(T))
            {
                @postmove(newBuffer, count)
            }
            #elif Reflection.canCopy(@typeof(T))
            {
                @postcopy(newBuffer, count)
                @drop(buffer, count)
            }
        }

        buffer = newBuffer
        capacity = newCapacity
    }

    // Returns true if the array is empty
    #[Inline]
    method isEmpty() => count == 0

    // Returns a slice
    method toSlice()->[..] T
    {
        return @mkslice(buffer, count)
    }

    // Ensure the Array is big enough to store at least 'newCount' elements
    // Number of valid elements does not change
    method grow(newCount : uint)
    {
        if newCount <= capacity
            return
        newCapacity := Math.max(capacity * 2, newCount)
        reserve(newCapacity)
    }

    // Reserve room for 'newCapacity' elements without changing the array count
    // Number of valid elements does not change
    method reserve(newCapacity: uint)
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
    method resize(newCount : uint)
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
    method clear()
    {
        #if Reflection.hasDrop(@typeof(T))
        {
            loop count
                @drop(buffer + @index)
        }

        count = 0
    }

    // Add a copy of one element at the end of the array
    method add(value: T)
        #selectif Reflection.canCopy(@typeof(T))
    {
        grow(count + 1)
        buffer[count] =,nodrop value
        count += 1
    }

    // Add any value, as long as the type T accepts an opAffect of type G
    // or an implicit cast
    method(G) addAny(value: G)
        #selectif Reflection.canCopy(@typeof(G))
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
    method emplaceAddress(num: u32)->*T
    {
        grow(count + num)
        count += num
        return buffer + count - num
    }

    // Move one element at the end of the array
    method emplace(value: *T)
    {
        grow(count + 1)
        buffer[count] =,nodrop,move dref value
        count += 1
    }

    // Insert a value at the given index. If 'index' is equal to 'count', then
    // the value is added at the end of the array.
    // Order is preserved.
    method insertAt(index: uint, value: T)
        #selectif Reflection.canCopy(@typeof(T))
    {
        Debug.assert(index <= count, "index out of range")
        if index == count
            return add(value)
        grow(count + 1)
        count += 1

        for idx := count - 1; idx > index; idx -= 1
            buffer[idx] =,nodrop,moveraw buffer[idx - 1]

        buffer[index] =,nodrop value
    }

    // Move a value at the given index. If 'index' is equal to 'count', then
    // the value is added at the end of the array.
    // Order is preserved.
    method emplaceAt(index: uint, value: *T)
    {
        Debug.assert(index <= count, "index out of range")
        if index == count
            return emplace(value)
        grow(count + 1)
        count += 1

        for idx := count - 1; idx > index; idx -= 1
            buffer[idx] =,nodrop,moveraw buffer[idx - 1]

        buffer[index] =,nodrop,move dref value
    }

    // Append a slice to the end of this instance
    method add(values: const [..] T)
        #selectif Reflection.canCopy(@typeof(T))
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

        #if Reflection.hasPostCopy(@typeof(T))
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

    // Remove an element at 'index' by replacing it with the last element.
    // Order is not preserved
    method removeAt(index: uint)
    {
        Debug.assert(index < count, "index out of range")
        count -= 1

        if index == count
        {
            @drop(&buffer[count])
            return
        }

        buffer[index] =,moveraw buffer[count]
    }

    // Remove an element at 'index' by shifting all others.
    // Order is preserved
    method removeAtOrdered(index: uint)
    {
        Debug.assert(index < count, "index out of range")
        count -= 1
        @drop(buffer + index)

        if index == count
            return

        idx := index
        loop count - index
        {
            buffer[idx] =,nodrop,moveraw buffer[idx + 1]
            idx += 1
        }
    }

    // Remove last element
    method removeBack()
    {
        Debug.assert(count > 0)
        @drop(&buffer[count - 1])
        count -= 1
    }

    // Returns 'true' if the array contains 'value'.
    // O(n)
    method contains(value: T)->bool
    {
        loop count
        {
            if(buffer[@index] == value)
                return true
        }

        return false
    }

    // Sort array
    method sort()
    {
        Slice.sort(@mkslice(buffer, count), @(x, y: T)->s32 => x <=> y)
    }

    // Sort array in reverse order (from biggest to lowest value)
    method sortReverse()
    {
        Slice.sort(@mkslice(buffer, count), @(x, y: T)->s32 => y <=> x)
    }

    // Sort array
    method sort(cb: func(T, T)->s32)
    {
        Debug.assert(cb != null)
        Slice.sort(@mkslice(buffer, count), cb)
    }

    // Returns the last element, and remove it from the array
    method back()->T
    {
        Debug.assert(count > 0)
        var result: retval = ?
        result =,nodrop,moveraw buffer[count - 1]
        return result
    }

    // Returns the last element, and remove it from the array
    method popBack()->T
    {
        Debug.assert(count > 0)
        var result: retval = ?
        result =,nodrop,moveraw buffer[count - 1]
        count -= 1
        return result
    }
}

impl StrConv.IConvert for Array
{
    method convert(buf: *ConcatBuffer, fmt: const *StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes("[")

        first := true
        bufArr := buffer
        loop count
        {
            if !first buf.addBytes(",")
            first = false
            v := @mkany(bufArr, T)
            StrConv.convertAny(buf, v, strFormat)
            bufArr += 1
        }

        buf.addBytes("]")
    }
}