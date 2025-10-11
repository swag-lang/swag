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
        return .count
    }

    // Returns a raw, constant pointer to the array's buffer.
    #[Inline]
    mtd opData()->const *T
    {
        return &.buffer[0]
    }

    // Ensures that if elements have destructors, they are called when the array goes out of scope.
    mtd opDrop()
    {
        .free()
    }

    // Allows the array to be implicitly cast to a mutable slice.
    #[Inline, Overload]
    mtd opCast()->[..] T
    {
        return .toSlice()
    }

    // Allows the array to be implicitly cast to an immutable slice.
    #[Inline, Overload]
    mtd opCast()->const [..] T
    {
        return .toSlice()
    }

    // Creates a slice from a sub-part of the array.
    #[Inline]
    mtd opSlice(lower, upper: u64)->[..] T
    {
        Debug.safetyBoundCheck(lower <= upper, "lower bound greater than upper bound")
        Debug.safetyBoundCheck(lower < .count, "lower bound out of range")
        Debug.safetyBoundCheck(upper < .count, "upper bound out of range")
        return @mkslice(&.buffer[0] + lower, (upper + 1) - lower)
    }

    // Returns a constant reference to the element at the specified index.
    #[Inline, Overload]
    mtd const opIndex(index: u64)->const &T
    {
        Debug.safetyBoundCheck(index < .count, "index out of range")
        return &.buffer[index]
    }

    // Returns a mutable reference to the element at the specified index.
    #[Inline, Overload]
    mtd opIndex(index: u64)->&T
    {
        Debug.safetyBoundCheck(index < .count, "index out of range")
        return &.buffer[index]
    }

    // Assigns a new value to the element at the specified index.
    #[Inline]
    mtd opIndexAffect(index: u64, value: T)
        where Reflection.canCopy(T)
    {
        Debug.safetyBoundCheck(index < .count, "index out of range")
        .buffer[index] = value
    }

    // Handles compound assignment operators (e.g., `+=`, `-=`) for elements.
    #[Inline]
    mtd(op: string) opIndexAssign(index: u64, value: T)
    {
        Debug.safetyBoundCheck(index < .count, "index out of range")
        #ast "buffer[index] " ++ op ++ " value"
    }

    // Replaces the content of the array with elements copied from a slice.
    // Asserts if the slice is larger than the array's capacity.
    mtd opAffect(arr: const [..] T)
        where Reflection.canCopy(T)
    {
        .clear()
        var countof = @countof(arr)
        if !countof do
            return
        Debug.assert(countof < N, "slice is too big")
        var dataof = @dataof(arr)

        #if Reflection.hasPostCopy(T)
        {
            for i in countof do
                .buffer[i] = #nodrop dataof[i]
        }
        #else
        {
            Memory.copy(.buffer, dataof, #sizeof(T) * countof)
        }

        .count += countof
    }

    // Provides a way to iterate over each element in the array.
    // This is a macro that accepts a code block to execute for each element.
    // Visiting by pointer and in reverse order is supported.
    #[Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        if !.count do
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work correctly
        #if back do
            var scan = &.buffer[0] + .count
        #else do
            var scan = &.buffer[0] - 1

        for .count
        {
            #if back do
                scan -= 1
            #else do
                scan += 1
            #macro
            {
                #if #up ptr do
                    let #alias0 = cast(*T) #up scan
                #elif Reflection.isStruct(T) do
                    let #alias0 = cast(&T) #up scan
                #else do
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
        if !.count do
            return

        // Need to drop every structs
        #if Reflection.hasDrop(T)
        {
            @drop(&.buffer[0], .count)
        }

        .count = 0
    }

    // Returns 'true' if the array contains no elements.
    #[Inline]
    mtd const isEmpty() => .count == 0

    // Returns a mutable slice containing all valid elements of the array.
    #[Inline]
    mtd toSlice()->[..] T
    {
        return @mkslice(&.buffer[0], .count)
    }

    // Changes the number of elements in the array to 'newCount'.
    // Asserts if 'newCount' exceeds the static capacity.
    mtd resize(newCount: u64)
    {
        if newCount == .count do
            return
        Debug.assert(newCount < N, "size is too big")
        if newCount > .count
        {
            @init(buffer + .count, newCount - .count)
            .count = newCount
        }
        else
        {
            @drop(buffer + newCount, .count - newCount)
            .count = newCount
        }
    }

    // Removes all elements from the array, setting its me.count to 0.
    // This calls the destructor for each element if one exists.
    mtd clear()
    {
        #if Reflection.hasDrop(T)
        {
            for .count do
                @drop(&.buffer[0] + @index)
        }

        .count = 0
    }

    // Adds an element to the end of the array, but only if it's not already present.
    // Asserts if the array is full.
    mtd addOnce(value: T)
        where Reflection.canCopy(T)
    {
        if .contains(value) do
            return
        Debug.assert(.count != N, "the array is full")
        .buffer[.count] = #nodrop value
        .count += 1
    }

    // Adds a copy of an element to the end of the array.
    // Asserts if the array is full.
    #[Overload]
    mtd add(value: T)
        where Reflection.canCopy(T)
    {
        Debug.assert(.count != N, "the array is full")
        .buffer[.count] = #nodrop value
        .count += 1
    }

    // Moves an element to the end of the array.
    // Asserts if the array is full.
    #[Overload]
    mtd add(value: &&T)
    {
        Debug.assert(.count != N, "the array is full")
        .buffer[.count] = #nodrop #move value
        .count += 1
    }

    // Makes room for 'num' elements at the end of the array and returns a pointer to the new space.
    // The new memory is not initialized. Asserts if the array does not have enough capacity.
    mtd emplaceAddress(num: u32 = 1)->[*] T
    {
        Debug.assert(.count + num <= N, "the array is full")
        .count += num
        return .buffer + .count - num
    }

    // Makes room for 'num' elements at the end of the array and returns a pointer to the new space.
    // The new memory is default-initialized. Asserts if the array does not have enough capacity.
    mtd emplaceInitAddress(num: u32 = 1)->[*] T
    {
        Debug.assert(.count + num <= N, "the array is full")
        .count += num
        var res = .buffer + .count - num
        @init(res, num)
        return res
    }

    // Inserts a copy of a value at a specific index, shifting existing elements to the right.
    // Asserts if the array is full. Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: T)
        where Reflection.canCopy(T)
    {
        Debug.assert(index <= .count, "index out of range")
        if index == .count do
            return .add(value)
        Debug.assert(.count != N, "the array is full")
        .count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = .count - 1; idx > index; idx -= 1 do
                .buffer[idx] = #nodrop #moveraw .buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(&.buffer[0] + index + 1, &.buffer[0] + index, #sizeof(T) * (.count - 1 - index))
        }

        .buffer[index] = #nodrop value
    }

    // Inserts a moved value at a specific index, shifting existing elements to the right.
    // Asserts if the array is full. Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: &&T)
    {
        Debug.assert(index <= .count, "index out of range")
        if index == .count do
            return .emplace(value)
        Debug.assert(.count != N, "the array is full")
        .count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = .count - 1; idx > index; idx -= 1 do
                .buffer[idx] = #nodrop #moveraw .buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(&.buffer[0] + index + 1, &.buffer[0] + index, #sizeof(T) * (.count - 1 - index))
        }

        .buffer[index] = #nodrop #move value
    }

    // Inserts elements from a slice at a specific index, shifting existing elements to the right.
    // Asserts if the array does not have enough capacity. Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, values: const [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return

        Debug.assert(index <= .count, "index out of range")
        if index == .count do
            return .add(values)
        Debug.assert(.count + sliceCount <= N, "the array is full")
        .count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = .count - 1; idx >= index + sliceCount; idx -= 1 do
                .buffer[idx] = #nodrop #moveraw .buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(&.buffer[0] + index + sliceCount, &.buffer[0] + index, #sizeof(T) * (.count - sliceCount - index))
        }

        #if Reflection.hasPostCopy(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1 do
                .buffer[index + idx] = #nodrop values[idx]
        }
        #else
        {
            Memory.copy(&.buffer[0] + index, @dataof(values), #sizeof(T) * sliceCount)
        }
    }

    // Moves elements from a slice at a specific index, shifting existing elements to the right.
    // Asserts if the array does not have enough capacity. Order is preserved.
    mtd emplaceAt(index: u64, values: [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return

        Debug.assert(index <= .count, "index out of range")
        if index == .count do
            return .add(values)
        Debug.assert(.count + sliceCount <= N, "the array is full")
        .count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = .count - 1; idx >= index + sliceCount; idx -= 1 do
                .buffer[idx] = #nodrop #moveraw .buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(&.buffer[0] + index + @countof(values), &.buffer[0] + index, #sizeof(T) * (.count - sliceCount - index))
        }

        #if Reflection.hasPostMove(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1 do
                .buffer[index + idx] = #nodrop #move values[idx]
        }
        #else
        {
            Memory.copy(&.buffer[0] + index, @dataof(values), #sizeof(T) * sliceCount)
        }
    }

    // Appends all elements from a slice to the end of the array.
    // Asserts if the array does not have enough capacity.
    #[Overload]
    mtd add(values: const [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return
        Debug.assert(.count + sliceCount <= N, "the array is full")
        var oldCount = .count
        .count += sliceCount

        #if Reflection.hasPostCopy(T)
        {
            var destPtr = &.buffer[0] + oldCount
            for sliceCount
            {
                dref destPtr = #nodrop values[@index]
                destPtr += 1
            }
        }
        #else
        {
            Memory.copy(&.buffer[0] + oldCount, @dataof(values), sliceCount * #sizeof(T))
        }
    }

    // Removes a specified number of elements starting from a given index.
    // This is an unstable remove; the order of remaining elements is not preserved.
    #[Overload]
    mtd remove(index, num: u64)
    {
        if !num do
            return
        Debug.assert(index + num <= .count, "index out of range")

        #if !Reflection.hasPostMove(T) and !Reflection.hasDrop(T)
        {
            Memory.copyOver(&.buffer[0] + index, &.buffer[0] + index + num, (.count - (index + num)) * #sizeof(T))
        }
        #else
        {
            for i in .count - num
            {
                .buffer[index + i] = #moveraw .buffer[index + num + i]
            }
        }

        .count -= num
    }

    // Removes the first occurrence of a given value from the array.
    // This is an unstable remove; order is not preserved. Does nothing if the value is not found.
    #[Overload]
    mtd(V) remove(value: V)
    {
        let idx = Slice.findLinear(.toSlice(), value)
        if idx == Swag.U64.Max do
            return
        .removeAt(idx)
    }

    // Removes the element at a given index by swapping it with the last element.
    // This is very fast, but the order of elements is not preserved.
    mtd removeAt(index: u64)
    {
        Debug.assert(index < .count, "index out of range")
        .count -= 1
        if index == .count do
            @drop(&.buffer[0] + .count)
        else do
            .buffer[index] = #moveraw .buffer[.count]
    }

    // Removes one or more elements at a given index, shifting subsequent elements to the left.
    // This is slower than 'removeAt', but preserves the order of the remaining elements.
    mtd removeAtOrdered(index: u64, numValues: u64 = 1)
    {
        if !numValues do
            return
        Debug.assert(index + numValues <= .count, "index out of range")
        .count -= numValues

        #if Reflection.hasDrop(T)
        {
            for idx in numValues do
                @drop(&.buffer[0] + index + idx)
        }

        if index >= .count do
            return

        #if Reflection.hasPostMove(T)
        {
            var idx = index
            for .count - index
            {
                .buffer[idx] = #nodrop #moveraw .buffer[idx + numValues]
                idx += 1
            }
        }
        #else
        {
            Memory.copyOver(&.buffer[0] + index, &.buffer[0] + index + numValues, #sizeof(T) * (.count - index))
        }
    }

    // Removes the first occurrence of a given value, preserving the order of remaining elements.
    // Does nothing if the value is not found.
    mtd(V) removeOrdered(value: V)
    {
        let idx = Slice.findLinear(.toSlice(), value)
        if idx == Swag.U64.Max do
            return
        .removeAtOrdered(idx)
    }

    // Returns 'true' if the array contains the given value.
    mtd(V) contains(value: V)->bool
    {
        return Slice.findLinear(.toSlice(), value) != Swag.U64.Max
    }

    // Sorts the array in ascending order using the default comparison operator.
    #[Swag.Overload]
    mtd sort()
    {
        Slice.sort(.toSlice(), func(x, y: T)->s32 => x <=> y)
    }

    // Sorts the array in descending order.
    mtd sortReverse()
    {
        Slice.sort(.toSlice(), func(x, y: T)->s32 => y <=> x)
    }

    // Sorts the array using a custom comparison function.
    #[Swag.Overload]
    mtd sort(cb: func||(T, T)->s32)
    {
        Debug.assert(cb != null)
        Slice.sort(.toSlice(), cb)
    }

    // Returns a copy of the first element of the array.
    // Asserts if the array is empty.
    mtd front()->T
    {
        Debug.assert(.count > 0, "array is empty")
        let result: retval = .buffer[0]
        return result
    }

    // Returns a pointer to the first element of the array.
    // Asserts if the array is empty.
    mtd frontPtr()->*T
    {
        Debug.assert(.count > 0, "array is empty")
        return &.buffer[0]
    }

    // Returns a copy of the last element of the array.
    // Asserts if the array is empty.
    mtd back()->T
    {
        Debug.assert(.count > 0, "array is empty")
        let result: retval = .buffer[.count - 1]
        return result
    }

    // Returns a pointer to the last element of the array.
    // Asserts if the array is empty.
    mtd backPtr()->*T
    {
        Debug.assert(.count > 0, "array is empty")
        return &.buffer[.count - 1]
    }

    // Removes the last element from the array and returns it.
    // Asserts if the array is empty.
    mtd popBack()->T
    {
        Debug.assert(.count > 0, "array is empty")
        var result: retval = undefined
        result = #nodrop #moveraw .buffer[.count - 1]
        .count -= 1
        return result
    }

    // Removes the last element from the array.
    // Asserts if the array is empty.
    mtd removeBack()
    {
        Debug.assert(.count > 0, "array is empty")
        @drop(&.buffer[0] + .count - 1)
        .count -= 1
    }
}
