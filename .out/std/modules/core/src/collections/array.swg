#global export
using swag

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

        #if reflection.hasPostCopy(@typeof(T))
        {
            loop count
                buffer[@index] = nodrop oldBuffer[@index]
        }
        #else
        {
            memory.copy(buffer, oldBuffer, @sizeof(T) * count)
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
        debug.assert(lower <= upper and lower < count and upper < count)
        return @mkslice(buffer + lower, (upper + 1) - lower)
    }

    func opIndex(using self, index: uint)->*T
        #selectif reflection.isStruct(T)
    {
        debug.assert(index < count, "index out of range")
        return buffer + index
    }

    func opIndex(using self, index: uint)->T
        #selectif !reflection.isStruct(T)
    {
        debug.assert(index < count, "index out of range")
        return buffer[index]
    }

    func opIndexAffect(using self, index: uint, value: T)
        #selectif reflection.canCopy(@typeof(T))
    {
        debug.assert(index < count, "index out of range")
        buffer[index] = value
    }

    func(op: string) opIndexAssign(using self, index: uint, value: T)
    {
        debug.assert(index < count, "index out of range")
        #ast "buffer[index] " ~op~ " value"
    }

    // Initializes an Array that contains values copied from the specified array
    func opAffect(using self, arr: const [..] T)
        #selectif reflection.canCopy(@typeof(T))
    {
        clear()
        countof := @countof(arr)
        if !countof return
        dataof := @dataof(arr)

        grow(countof)
        #if reflection.hasPostCopy(@typeof(T))
        {
            loop i: countof
                buffer[i] = nodrop dataof[i]
        }
        #else
        {
            memory.copy(buffer, dataof, @sizeof(T) * countof)
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
                    var @alias0 = reflection.isStruct(T) ? cast(const *T) `scan : `scan[0]
                var @alias1 = @index
                #mixin `stmt
            }
        }
    }

    func free(using self)
    {
        if !buffer
            return

        // Need to drop every structs
        #if reflection.hasDrop(@typeof(T))
        {
            loop count
                @drop(buffer + @index)
        }

        memory.free(buffer, capacity * @sizeof(T), allocator)
    }

    func realloc(using self, newCapacity: uint)
    {
        if allocator == null
            allocator = @getcontext().allocator

        newBuffer := cast(*T) memory.realloc(buffer, newCapacity * @sizeof(T), capacity * @sizeof(T), allocator)

        // Need to call opPostMove/opPostCopy on every existing element if the buffer has not been reallocated
        // in place
        if buffer and newBuffer != buffer
        {
            @reloc(newBuffer, buffer, count)
            #if reflection.hasPostMove(@typeof(T))
            {
                @postmove(newBuffer, count)
            }
            #elif reflection.canCopy(@typeof(T))
            {
                @postcopy(newBuffer, count)
                @drop(buffer, count)
            }
        }

        buffer = newBuffer
        capacity = newCapacity
    }

    // Returns true if the array is empty
    #[inline]
    func isEmpty(using self) => count == 0

    // Returns a slice
    func toSlice(using self)->[..] T
    {
        return @mkslice(buffer, count)
    }

    // Ensure the Array is big enough to store at least 'newCount' elements
    // Number of valid elements does not change
    func grow(using self, newCount : uint)
    {
        if newCount <= capacity
            return
        newCapacity := math.max(capacity * 2, newCount)
        reserve(newCapacity)
    }

    // Reserve room for 'newCapacity' elements without changing the array count
    // Number of valid elements does not change
    func reserve(using self, newCapacity: uint)
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
    func resize(using self, newCount : uint)
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
    func clear(using self)
    {
        #if reflection.hasDrop(@typeof(T))
        {
            loop count
                @drop(buffer + @index)
        }

        count = 0
    }

    // Add a copy of one element at the end of the array
    func add(using self, value: T)
        #selectif reflection.canCopy(@typeof(T))
    {
        grow(count + 1)
        buffer[count] = nodrop value
        count += 1
    }

    // Add any value, as long as the type T accepts an opAffect of type G
    // or an implicit cast
    func(G) addAny(using self, value: G)
        #selectif reflection.canCopy(@typeof(G))
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

    // Move one element at the end of the array
    func emplace(using self, value: *T)
    {
        grow(count + 1)
        buffer[count] = nodrop move :value
        count += 1
    }

    // Insert a value at the given index. If 'index' is equal to 'count', then
    // the value is added at the end of the array.
    // Order is preserved.
    func insertAt(using self, index: uint, value: T)
        #selectif reflection.canCopy(@typeof(T))
    {
        debug.assert(index <= count, "index out of range")
        if index == count
            return add(value)
        grow(count + 1)
        count += 1

        for idx := count - 1; idx > index; idx -= 1
            buffer[idx] = nodrop move nodrop buffer[idx - 1]

        buffer[index] = nodrop value
    }

    // Move a value at the given index. If 'index' is equal to 'count', then
    // the value is added at the end of the array.
    // Order is preserved.
    func emplaceAt(using self, index: uint, value: *T)
    {
        debug.assert(index <= count, "index out of range")
        if index == count
            return emplace(value)
        grow(count + 1)
        count += 1

        for idx := count - 1; idx > index; idx -= 1
            buffer[idx] = nodrop move nodrop buffer[idx - 1]

        buffer[index] = nodrop move :value
    }

    // Append a slice to the end of this instance
    func add(using self, values: const [..] T)
        #selectif reflection.canCopy(@typeof(T))
    {
        sliceCount := @countof(values)
        if !sliceCount
            return

        // Do not add myself if there's not enough room (because we will reallocate)
        if count + sliceCount > capacity
            debug.assert(@dataof(values) < buffer or @dataof(values) > buffer + capacity)

        grow(count + sliceCount)
        oldCount := count
        count += sliceCount

        #if reflection.hasPostCopy(@typeof(T))
        {
            destPtr := buffer + oldCount
            loop sliceCount
            {
                :destPtr = nodrop values[@index]
                destPtr += 1
            }
        }
        #else
        {
            memory.copy(buffer + oldCount, @dataof(values), sliceCount * @sizeof(T))
        }
    }

    // Remove an element at 'index' by replacing it with the last element.
    // Order is not preserved
    func removeAt(using self, index: uint)
    {
        debug.assert(index < count, "index out of range")
        count -= 1

        if index == count
        {
            @drop(&buffer[count])
            return
        }

        buffer[index] = move nodrop buffer[count]
    }

    // Remove an element at 'index' by shifting all others.
    // Order is preserved
    func removeAtOrdered(using self, index: uint)
    {
        debug.assert(index < count, "index out of range")
        count -= 1
        @drop(buffer + index)

        if index == count
            return

        idx := index
        loop count - index
        {
            buffer[idx] = nodrop move nodrop buffer[idx + 1]
            idx += 1
        }
    }

    // Remove last element
    func removeBack(using self)
    {
        debug.assert(count > 0)
        @drop(&buffer[count - 1])
        count -= 1
    }

    // Returns 'true' if the array contains 'value'.
    // O(n)
    func contains(using self, value: T)->bool
    {
        loop count
        {
            if(buffer[@index] == value)
                return true
        }

        return false
    }

    // Sort array
    func sort(using self, cb: func(T, T)->s32 = null)
    {
        lambda := cb orelse @(x, y: T)->s32 => x <=> y
        slice.sort(@mkslice(buffer, count), lambda)
    }

    // Returns the last element, and remove it from the array
    func popBack(using self)->T
    {
        debug.assert(count > 0)
        var result: retval = ?
        result = nodrop move nodrop buffer[count - 1]
        count -= 1
        return result
    }
}

impl strconv.IConvert for Array
{
    func convert(using self, buf: *ConcatBuffer, fmt: const *strconv.ConvertFormat, strFormat: string)
    {
        buf.addBytes("[")

        first := true
        bufArr := buffer
        loop count
        {
            if !first buf.addBytes(",")
            first = false
            v := @mkany(bufArr, T)
            strconv.convertAny(buf, v, strFormat)
            bufArr += 1
        }

        buf.addBytes("]")
    }
}