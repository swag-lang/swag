#public
namespace std;
using swag

struct(T) Array
{
    allocator:  const IAllocator
    buffer:     *T
    count:      u32
    capacity:   u32
}

impl Array
{
    func opCount(using self)->u32
    {
        return count
    }

    func opData(using self)->const *void
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

    func opSlice(using self, lower, upper: u32)->[..] T
    {
        debug.assert(lower <= upper && lower < count && upper < count)
        return @mkslice(buffer + lower, (upper + 1) - lower)
    }

    func opIndex(using self, index: u32)->const &T
    {
        debug.assert(index < count, "index out of range")
        return buffer[index]
    }

    func opIndexAffect(using self, index: u32, value: T)
    {
        debug.assert(index < count, "index out of range")
        buffer[index] = value
    }

    // Initializes an Array that contains values copied from the specified array
    func opAffect(using self, arr: const [..] T)
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
    #[macro, noreturn]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        if count
        {
            scan := buffer
            loop count
            {
                #macro
                {
                    #if ptr
                        var @alias0 = `scan
                    #else
                        var @alias0 = :`scan
                    var @alias1 = @index
                    #mixin `stmt
                }

                scan += 1
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

        memory.free(&allocator, buffer)
    }

    func realloc(using self, newCapacity: u32)
    {
        if allocator == null
            allocator = @getcontext().allocator

        newBuffer := cast(*T) memory.realloc(&allocator, buffer, newCapacity * @sizeof(T))

        // Need to call opPostMove/opPostCopy on every existing element if the buffer has not been reallocated
        // in place
        if buffer && newBuffer != buffer
        {
            #if reflection.hasPostMove(@typeof(T))
            {
                @postmove(newBuffer, count)
            }
            #elif reflection.hasPostCopy(@typeof(T))
            {
                @postcopy(newBuffer, count)
                @drop(buffer, count)
            }
            #elif reflection.hasDrop(@typeof(T))
            {
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
    func grow(using self, newCount : u32)
    {
        if newCount <= capacity
            return
        newCapacity := math.max(capacity * 2, newCount)
        reserve(newCapacity)
    }

    // Reserve room for 'newCapacity' elements without changing the array count
    func reserve(using self, newCapacity: u32)
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
    {
        grow(count + 1)
        buffer[count] = nodrop value
        count += 1
    }

    // Add any value, as long as the type T accepts an opAffect of type G
    // or an implicit cast
    func(G) addAny(using self, value: G)
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
    func insertAt(using self, index: u32, value: T)
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
    func emplaceAt(using self, index: u32, value: *T)
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

    // Append another array to the end of this instance
    func add(using self, array: Array)
    {
        // Do not add myself !
        debug.assert(array.buffer != buffer)

        add(@mkslice(array.buffer, array.count))
    }

    // Append a slice to the end of this instance
    func add(using self, values: const [..] T)
    {
        // Do not add myself !
        debug.assert(@dataof(values) != buffer)

        sliceCount := @countof(values)
        if !sliceCount
            return
        grow(count + sliceCount)

        oldCount := count
        count += sliceCount

        loop sliceCount
        {
            buffer[oldCount] = nodrop values[@index]
            oldCount += 1
        }
    }

    // Remove an element at 'index' by replacing it with the last element.
    // Order is not preserved
    func removeAt(using self, index: u32)
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
    func removeAtOrdered(using self, index: u32)
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
        lambda := cb ?? @(x, y: T)->s32 => x <=> y
        sort.slice(@mkslice(buffer, count), lambda)
    }

    // Returns the last element
    func back(using self)->const &T
    {
        debug.assert(count > 0)
        return buffer[count - 1]
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