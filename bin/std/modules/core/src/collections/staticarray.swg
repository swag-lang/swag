#global export
using Swag

// A stack-allocated array with a fixed capacity known at compile time.
// It provides a similar interface to the dynamic 'Array' but does not allocate memory on the heap.
struct(T, N: u64) StaticArray
{
    buffer:     [N] T     // The fixed-size me.buffer that holds the elements.
    count:      u64       // The number of valid elements currently in the array.
}

impl StaticArray
{
    // Returns the number of elements in the array.
    #[Inline]
    mtd opCount()->u64
    {
        return me.count
    }

    // Returns a raw, constant pointer to the array's buffer.
    #[Inline]
    mtd opData()->const *T
    {
        return &me.buffer[0]
    }

    // Ensures that if elements have destructors, they are called when the array goes out of scope.
    mtd opDrop()
    {
        me.free()
    }

    // Allows the array to be implicitly cast to a mutable slice.
    #[Inline, Overload]
    mtd opCast()->[..] T
    {
        return me.toSlice()
    }

    // Allows the array to be implicitly cast to an immutable slice.
    #[Inline, Overload]
    mtd opCast()->const [..] T
    {
        return me.toSlice()
    }

    // Creates a slice from a sub-part of the array.
    #[Inline]
    mtd opSlice(lower, upper: u64)->[..] T
    {
        Debug.safetyBoundCheck(lower <= upper, "lower bound greater than upper bound")
        Debug.safetyBoundCheck(lower < me.count, "lower bound out of range")
        Debug.safetyBoundCheck(upper < me.count, "upper bound out of range")
        return @mkslice(&me.buffer[0] + lower, (upper + 1) - lower)
    }

    // Returns a constant reference to the element at the specified index.
    #[Inline, Overload]
    mtd const opIndex(index: u64)->const &T
    {
        Debug.safetyBoundCheck(index < me.count, "index out of range")
        return &me.buffer[index]
    }

    // Returns a mutable reference to the element at the specified index.
    #[Inline, Overload]
    mtd opIndex(index: u64)->&T
    {
        Debug.safetyBoundCheck(index < me.count, "index out of range")
        return &me.buffer[index]
    }

    // Assigns a new value to the element at the specified index.
    #[Inline]
    mtd opIndexAffect(index: u64, value: T)
        where Reflection.canCopy(T)
    {
        Debug.safetyBoundCheck(index < me.count, "index out of range")
        me.buffer[index] = value
    }

    // Handles compound assignment operators (e.g., `+=`, `-=`) for elements.
    #[Inline]
    mtd(op: string) opIndexAssign(index: u64, value: T)
    {
        Debug.safetyBoundCheck(index < me.count, "index out of range")
        #ast "buffer[index] " ++ op ++ " value"
    }

    // Replaces the content of the array with elements copied from a slice.
    // Asserts if the slice is larger than the array's capacity.
    mtd opAffect(arr: const [..] T)
        where Reflection.canCopy(T)
    {
        me.clear()
        var countof = @countof(arr)
        if !countof:
            return
        Debug.assert(countof < N, "slice is too big")
        var dataof = @dataof(arr)

        #if Reflection.hasPostCopy(T)
        {
            for i in countof:
                me.buffer[i] = #nodrop dataof[i]
        }
        #else
        {
            Memory.copy(me.buffer, dataof, #sizeof(T) * countof)
        }

        me.count += countof
    }

    // Provides a way to iterate over each element in the array.
    // This is a macro that accepts a code block to execute for each element.
    // Visiting by pointer and in reverse order is supported.
    #[Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        if !me.count:
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work correctly
        #if back:
            var scan = &me.buffer[0] + me.count
        #else:
            var scan = &me.buffer[0] - 1

        for me.count
        {
            #if back:
                scan -= 1
            #else:
                scan += 1
            #macro
            {
                #if #up ptr:
                    let #alias0 = cast(*T) #up scan
                #elif Reflection.isStruct(T):
                    let #alias0 = cast(&T) #up scan
                #else:
                    let #alias0 = #up scan[0]
                let #alias1 = @index
                #inject(#up stmt)
            }
        }
    }

    // Drops all elements in the array, setting its me.count to 0.
    // This is equivalent to 'clear' for this type.
    mtd free()
    {
        if !me.count:
            return

        // Need to drop every structs
        #if Reflection.hasDrop(T)
        {
            @drop(&me.buffer[0], me.count)
        }

        me.count = 0
    }

    // Returns 'true' if the array contains no elements.
    #[Inline]
    mtd const isEmpty() => me.count == 0

    // Returns a mutable slice containing all valid elements of the array.
    #[Inline]
    mtd toSlice()->[..] T
    {
        return @mkslice(&me.buffer[0], me.count)
    }

    // Changes the number of elements in the array to 'newCount'.
    // Asserts if 'newCount' exceeds the static capacity.
    mtd resize(newCount: u64)
    {
        if newCount == me.count:
            return
        Debug.assert(newCount < N, "size is too big")
        if newCount > me.count
        {
            @init(buffer + me.count, newCount - me.count)
            me.count = newCount
        }
        else
        {
            @drop(buffer + newCount, me.count - newCount)
            me.count = newCount
        }
    }

    // Removes all elements from the array, setting its me.count to 0.
    // This calls the destructor for each element if one exists.
    mtd clear()
    {
        #if Reflection.hasDrop(T)
        {
            for me.count:
                @drop(&me.buffer[0] + @index)
        }

        me.count = 0
    }

    // Adds an element to the end of the array, but only if it's not already present.
    // Asserts if the array is full.
    mtd addOnce(value: T)
        where Reflection.canCopy(T)
    {
        if me.contains(value):
            return
        Debug.assert(me.count != N, "the array is full")
        me.buffer[me.count] = #nodrop value
        me.count += 1
    }

    // Adds a copy of an element to the end of the array.
    // Asserts if the array is full.
    #[Overload]
    mtd add(value: T)
        where Reflection.canCopy(T)
    {
        Debug.assert(me.count != N, "the array is full")
        me.buffer[me.count] = #nodrop value
        me.count += 1
    }

    // Moves an element to the end of the array.
    // Asserts if the array is full.
    #[Overload]
    mtd add(value: &&T)
    {
        Debug.assert(me.count != N, "the array is full")
        me.buffer[me.count] = #nodrop #move value
        me.count += 1
    }

    // Makes room for 'num' elements at the end of the array and returns a pointer to the new space.
    // The new memory is not initialized. Asserts if the array does not have enough capacity.
    mtd emplaceAddress(num: u32 = 1)->^T
    {
        Debug.assert(me.count + num <= N, "the array is full")
        me.count += num
        return me.buffer + me.count - num
    }

    // Makes room for 'num' elements at the end of the array and returns a pointer to the new space.
    // The new memory is default-initialized. Asserts if the array does not have enough capacity.
    mtd emplaceInitAddress(num: u32 = 1)->^T
    {
        Debug.assert(me.count + num <= N, "the array is full")
        me.count += num
        var res = me.buffer + me.count - num
        @init(res, num)
        return res
    }

    // Inserts a copy of a value at a specific index, shifting existing elements to the right.
    // Asserts if the array is full. Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: T)
        where Reflection.canCopy(T)
    {
        Debug.assert(index <= me.count, "index out of range")
        if index == me.count:
            return me.add(value)
        Debug.assert(me.count != N, "the array is full")
        me.count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = me.count - 1; idx > index; idx -= 1:
                me.buffer[idx] = #nodrop #moveraw me.buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(&me.buffer[0] + index + 1, &me.buffer[0] + index, #sizeof(T) * (me.count - 1 - index))
        }

        me.buffer[index] = #nodrop value
    }

    // Inserts a moved value at a specific index, shifting existing elements to the right.
    // Asserts if the array is full. Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: &&T)
    {
        Debug.assert(index <= me.count, "index out of range")
        if index == me.count:
            return me.emplace(value)
        Debug.assert(me.count != N, "the array is full")
        me.count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = me.count - 1; idx > index; idx -= 1:
                me.buffer[idx] = #nodrop #moveraw me.buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(&me.buffer[0] + index + 1, &me.buffer[0] + index, #sizeof(T) * (me.count - 1 - index))
        }

        me.buffer[index] = #nodrop #move value
    }

    // Inserts elements from a slice at a specific index, shifting existing elements to the right.
    // Asserts if the array does not have enough capacity. Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, values: const [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount:
            return

        Debug.assert(index <= me.count, "index out of range")
        if index == me.count:
            return me.add(values)
        Debug.assert(me.count + sliceCount <= N, "the array is full")
        me.count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = me.count - 1; idx >= index + sliceCount; idx -= 1:
                me.buffer[idx] = #nodrop #moveraw me.buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(&me.buffer[0] + index + sliceCount, &me.buffer[0] + index, #sizeof(T) * (me.count - sliceCount - index))
        }

        #if Reflection.hasPostCopy(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1:
                me.buffer[index + idx] = #nodrop values[idx]
        }
        #else
        {
            Memory.copy(&me.buffer[0] + index, @dataof(values), #sizeof(T) * sliceCount)
        }
    }

    // Moves elements from a slice at a specific index, shifting existing elements to the right.
    // Asserts if the array does not have enough capacity. Order is preserved.
    mtd emplaceAt(index: u64, values: [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount:
            return

        Debug.assert(index <= me.count, "index out of range")
        if index == me.count:
            return me.add(values)
        Debug.assert(me.count + sliceCount <= N, "the array is full")
        me.count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = me.count - 1; idx >= index + sliceCount; idx -= 1:
                me.buffer[idx] = #nodrop #moveraw me.buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(&me.buffer[0] + index + @countof(values), &me.buffer[0] + index, #sizeof(T) * (me.count - sliceCount - index))
        }

        #if Reflection.hasPostMove(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1:
                me.buffer[index + idx] = #nodrop #move values[idx]
        }
        #else
        {
            Memory.copy(&me.buffer[0] + index, @dataof(values), #sizeof(T) * sliceCount)
        }
    }

    // Appends all elements from a slice to the end of the array.
    // Asserts if the array does not have enough capacity.
    #[Overload]
    mtd add(values: const [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount:
            return
        Debug.assert(me.count + sliceCount <= N, "the array is full")
        var oldCount = me.count
        me.count += sliceCount

        #if Reflection.hasPostCopy(T)
        {
            var destPtr = &me.buffer[0] + oldCount
            for sliceCount
            {
                dref destPtr = #nodrop values[@index]
                destPtr += 1
            }
        }
        #else
        {
            Memory.copy(&me.buffer[0] + oldCount, @dataof(values), sliceCount * #sizeof(T))
        }
    }

    // Removes a specified number of elements starting from a given index.
    // This is an unstable remove; the order of remaining elements is not preserved.
    #[Overload]
    mtd remove(index, num: u64)
    {
        if !num:
            return
        Debug.assert(index + num <= me.count, "index out of range")

        #if !Reflection.hasPostMove(T) and !Reflection.hasDrop(T)
        {
            Memory.copyOver(&me.buffer[0] + index, &me.buffer[0] + index + num, (me.count - (index + num)) * #sizeof(T))
        }
        #else
        {
            for i in me.count - num
            {
                me.buffer[index + i] = #moveraw me.buffer[index + num + i]
            }
        }

        me.count -= num
    }

    // Removes the first occurrence of a given value from the array.
    // This is an unstable remove; order is not preserved. Does nothing if the value is not found.
    #[Overload]
    mtd(V) remove(value: V)
    {
        let idx = Slice.findLinear(me.toSlice(), value)
        if idx == Swag.U64.Max:
            return
        me.removeAt(idx)
    }

    // Removes the element at a given index by swapping it with the last element.
    // This is very fast, but the order of elements is not preserved.
    mtd removeAt(index: u64)
    {
        Debug.assert(index < me.count, "index out of range")
        me.count -= 1
        if index == me.count:
            @drop(&me.buffer[0] + me.count)
        else:
            me.buffer[index] = #moveraw me.buffer[me.count]
    }

    // Removes one or more elements at a given index, shifting subsequent elements to the left.
    // This is slower than 'removeAt', but preserves the order of the remaining elements.
    mtd removeAtOrdered(index: u64, numValues: u64 = 1)
    {
        if !numValues:
            return
        Debug.assert(index + numValues <= me.count, "index out of range")
        me.count -= numValues

        #if Reflection.hasDrop(T)
        {
            for idx in numValues:
                @drop(&me.buffer[0] + index + idx)
        }

        if index >= me.count:
            return

        #if Reflection.hasPostMove(T)
        {
            var idx = index
            for me.count - index
            {
                me.buffer[idx] = #nodrop #moveraw me.buffer[idx + numValues]
                idx += 1
            }
        }
        #else
        {
            Memory.copyOver(&me.buffer[0] + index, &me.buffer[0] + index + numValues, #sizeof(T) * (me.count - index))
        }
    }

    // Removes the first occurrence of a given value, preserving the order of remaining elements.
    // Does nothing if the value is not found.
    mtd(V) removeOrdered(value: V)
    {
        let idx = Slice.findLinear(me.toSlice(), value)
        if idx == Swag.U64.Max:
            return
        me.removeAtOrdered(idx)
    }

    // Returns 'true' if the array contains the given value.
    mtd(V) contains(value: V)->bool
    {
        return Slice.findLinear(me.toSlice(), value) != Swag.U64.Max
    }

    // Sorts the array in ascending order using the default comparison operator.
    #[Swag.Overload]
    mtd sort()
    {
        Slice.sort(me.toSlice(), func(x, y: T)->s32 => x <=> y)
    }

    // Sorts the array in descending order.
    mtd sortReverse()
    {
        Slice.sort(me.toSlice(), func(x, y: T)->s32 => y <=> x)
    }

    // Sorts the array using a custom comparison function.
    #[Swag.Overload]
    mtd sort(cb: func||(T, T)->s32)
    {
        Debug.assert(cb != null)
        Slice.sort(me.toSlice(), cb)
    }

    // Returns a copy of the first element of the array.
    // Asserts if the array is empty.
    mtd front()->T
    {
        Debug.assert(me.count > 0, "array is empty")
        let result: retval = me.buffer[0]
        return result
    }

    // Returns a pointer to the first element of the array.
    // Asserts if the array is empty.
    mtd frontPtr()->*T
    {
        Debug.assert(me.count > 0, "array is empty")
        return &me.buffer[0]
    }

    // Returns a copy of the last element of the array.
    // Asserts if the array is empty.
    mtd back()->T
    {
        Debug.assert(me.count > 0, "array is empty")
        let result: retval = me.buffer[me.count - 1]
        return result
    }

    // Returns a pointer to the last element of the array.
    // Asserts if the array is empty.
    mtd backPtr()->*T
    {
        Debug.assert(me.count > 0, "array is empty")
        return &me.buffer[me.count - 1]
    }

    // Removes the last element from the array and returns it.
    // Asserts if the array is empty.
    mtd popBack()->T
    {
        Debug.assert(me.count > 0, "array is empty")
        var result: retval = undefined
        result = #nodrop #moveraw me.buffer[me.count - 1]
        me.count -= 1
        return result
    }

    // Removes the last element from the array.
    // Asserts if the array is empty.
    mtd removeBack()
    {
        Debug.assert(me.count > 0, "array is empty")
        @drop(&me.buffer[0] + me.count - 1)
        me.count -= 1
    }
}
