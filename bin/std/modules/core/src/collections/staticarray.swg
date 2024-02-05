#global export
using Swag

struct(T, N: u64) StaticArray
{
    buffer: [N] T
    count:  u64
}

impl StaticArray
{
    #[Inline]
    mtd opCount()->u64
    {
        return count
    }

    #[Inline]
    mtd opData()->const *T
    {
        return &buffer[0]
    }

    mtd opDrop()
    {
        free()
    }

    #[Inline, Overload]
    mtd opCast()->[..] T
    {
        return toSlice()
    }

    #[Inline, Overload]
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
        return @mkslice(&buffer[0] + lower, (upper + 1) - lower)
    }

    #[Inline, Overload]
    mtd const opIndex(index: u64)->const &T
    {
        Debug.safetyBoundCheck(index < count, "index out of range")
        return &buffer[index]
    }

    #[Inline, Overload]
    mtd opIndex(index: u64)->&T
    {
        Debug.safetyBoundCheck(index < count, "index out of range")
        return &buffer[index]
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
        if !countof do
            return
        Debug.assert(countof < N, "slice is too big")
        var dataof = @dataof(arr)

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
    // Visiting by pointer and in reverse order is supported
    #[Macro]
    func(ptr: bool, back: bool) opVisit(using const self, stmt: code)
    {
        if !count do
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work correctly
        #if back #do
            var scan = &buffer[0] + count
        #else #do
            var scan = &buffer[0] - 1

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

    // Free the array content
    mtd free()
    {
        if !count do
            return

        // Need to drop every structs
        #if Reflection.hasDrop(T)
        {
            @drop(&buffer[0], count)
        }

        count = 0
    }

    // Returns true if the array is empty
    #[Inline]
    mtd const isEmpty() => count == 0

    // Returns a slice
    #[Inline]
    mtd toSlice()->[..] T
    {
        return @mkslice(&buffer[0], count)
    }

    // Change the number of valid elements in the array
    mtd resize(newCount: u64)
    {
        if newCount == count do
            return
        Debug.assert(newCount < N, "size is too big")
        if newCount > count
        {
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
                @drop(&buffer[0] + #index)
        }

        count = 0
    }

    // Add a copy of one element at the end of the array
    mtd addOnce(value: T)
        #validif Reflection.canCopy(T)
    {
        if contains(value) do
            return
        Debug.assert(count != N, "the array is full")
        buffer[count] =,nodrop value
        count += 1
    }

    // Add a copy of one element at the end of the array
    #[Overload]
    mtd add(value: T)
        #validif Reflection.canCopy(T)
    {
        Debug.assert(count != N, "the array is full")
        buffer[count] =,nodrop value
        count += 1
    }

    // Move one element at the end of the array
    #[Overload]
    mtd add(value: &&T)
    {
        Debug.assert(count != N, "the array is full")
        buffer[count] =,nodrop,move value
        count += 1
    }

    // Reserve room at the end of the array for 'num' elements.
    // Does not initialize them. Returns the address of the first element
    mtd emplaceAddress(num: u32 = 1)->^T
    {
        Debug.assert(count + num <= N, "the array is full")
        count += num
        return buffer + count - num
    }

    // Reserve room at the end of the array for 'num' elements.
    // Returns the address of the first element
    mtd emplaceInitAddress(num: u32 = 1)->^T
    {
        Debug.assert(count + num <= N, "the array is full")
        count += num
        var res = buffer + count - num
        @init(res, num)
        return res
    }

    // Insert a value at the given index.
    // If 'index' is equal to 'count', then the value is added at the end of the array.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: T)
        #validif Reflection.canCopy(T)
    {
        Debug.assert(index <= count, "index out of range")
        if index == count do
            return add(value)
        Debug.assert(count != N, "the array is full")
        count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = count - 1; idx > index; idx -= 1 do
                buffer[idx] =,nodrop,moveraw buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(&buffer[0] + index + 1, &buffer[0] + index, @sizeof(T) * (count - 1 - index))
        }

        buffer[index] =,nodrop value
    }

    // Move a value at the given index.
    // If 'index' is equal to 'count', then the value is added at the end of the array.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: &&T)
    {
        Debug.assert(index <= count, "index out of range")
        if index == count do
            return emplace(value)
        Debug.assert(count != N, "the array is full")
        count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = count - 1; idx > index; idx -= 1 do
                buffer[idx] =,nodrop,moveraw buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(&buffer[0] + index + 1, &buffer[0] + index, @sizeof(T) * (count - 1 - index))
        }

        buffer[index] =,nodrop,move value
    }

    // Insert some values at the given index.
    // If 'index' is equal to 'count', then the values are added at the end of the array.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, values: const [..] T)
        #validif Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return

        Debug.assert(index <= count, "index out of range")
        if index == count do
            return add(values)
        Debug.assert(count + sliceCount <= N, "the array is full")
        count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = count - 1; idx >= index + sliceCount; idx -= 1 do
                buffer[idx] =,nodrop,moveraw buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(&buffer[0] + index + sliceCount, &buffer[0] + index, @sizeof(T) * (count - sliceCount - index))
        }

        #if Reflection.hasPostCopy(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1 do
                buffer[index + idx] =,nodrop values[idx]
        }
        #else
        {
            Memory.copy(&buffer[0] + index, @dataof(values), @sizeof(T) * sliceCount)
        }
    }

    // Move some values at the given index.
    // If 'index' is equal to 'count', then the values are moved at the end of the array.
    // Order is preserved.
    mtd emplaceAt(index: u64, values: [..] T)
        #validif Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return

        Debug.assert(index <= count, "index out of range")
        if index == count do
            return add(values)
        Debug.assert(count + sliceCount <= N, "the array is full")
        count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = count - 1; idx >= index + sliceCount; idx -= 1 do
                buffer[idx] =,nodrop,moveraw buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(&buffer[0] + index + @countof(values), &buffer[0] + index, @sizeof(T) * (count - sliceCount - index))
        }

        #if Reflection.hasPostMove(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1 do
                buffer[index + idx] =,nodrop,move values[idx]
        }
        #else
        {
            Memory.copy(&buffer[0] + index, @dataof(values), @sizeof(T) * sliceCount)
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
        Debug.assert(count + sliceCount <= N, "the array is full")
        var oldCount = count
        count += sliceCount

        #if Reflection.hasPostCopy(T)
        {
            var destPtr = &buffer[0] + oldCount
            loop sliceCount
            {
                dref destPtr =,nodrop values[#index]
                destPtr += 1
            }
        }
        #else
        {
            Memory.copy(&buffer[0] + oldCount, @dataof(values), sliceCount * @sizeof(T))
        }
    }

    // Remove 'num' elements starting at 'index'
    #[Overload]
    mtd remove(index, num: u64)
    {
        if !num do return
        Debug.assert(index + num <= count, "index out of range")

        #if !Reflection.hasPostMove(T) and !Reflection.hasDrop(T)
        {
            Memory.copyOver(&buffer[0] + index, &buffer[0] + index + num, (count - (index + num)) * @sizeof(T))
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
    // Order is not preserved
    mtd removeAt(index: u64)
    {
        Debug.assert(index < count, "index out of range")
        count -= 1
        if index == count do
            @drop(&buffer[0] + count)
        else do
            buffer[index] =,moveraw buffer[count]
    }

    // Remove 'numValues' elements at 'index' by shifting all others.
    // Order is preserved
    mtd removeAtOrdered(index: u64, numValues: u64 = 1)
    {
        if !numValues do
            return
        Debug.assert(index + numValues <= count, "index out of range")
        count -= numValues

        #if Reflection.hasDrop(T)
        {
            loop idx: numValues do
                @drop(&buffer[0] + index + idx)
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
            Memory.copyOver(&buffer[0] + index, &buffer[0] + index + numValues, @sizeof(T) * (count - index))
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
    #[Swag.Overload]
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
    #[Swag.Overload]
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
        return &buffer[0]
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
        return &buffer[count - 1]
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
        @drop(&buffer[0] + count - 1)
        count -= 1
    }
}