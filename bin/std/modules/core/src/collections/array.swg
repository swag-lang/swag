#global export
using Swag

// A generic, dynamic array that can hold elements of any type.
// It automatically manages memory and grows as needed.
struct(T) Array
{
    allocator:     IAllocator     // The allocator used for memory operations.
    buffer:        [*] T          // Pointer to the contiguous memory block holding the elements.
    count:         u64            // The number of valid elements currently in the array.
    capacity:      u64            // The total number of elements that can be stored in the allocated me.buffer.
}

impl Array
{
    // Returns the number of elements in the array.
    #[Inline]
    mtd opCount()->u64
    {
        return .count
    }

    // Returns a raw, constant pointer to the array's me.buffer.
    #[Inline]
    mtd opData()->const *T
    {
        return .buffer
    }

    // Ensures the allocated memory is freed when the array goes out of scope.
    mtd opDrop()
    {
        .free()
    }

    // Handles the deep copy of the array when it is copied.
    // A new me.buffer is allocated and the content is copied over.
    mtd opPostCopy()
    {
        if .buffer == null do
            return

        let oldBuffer = .buffer
        .buffer = null
        .realloc(.count)

        #if Reflection.hasPostCopy(T)
        {
            for .count do
                .buffer[@index] = #nodrop oldBuffer[@index]
        }
        #else
        {
            Memory.copy(.buffer, oldBuffer, #sizeof(T) * .count)
        }
    }

    // Allows the array to be implicitly cast to a mutable slice.
    #[Inline, Overload]
    mtd opCast()->[..] T
    {
        return .toSlice()
    }

    // Allows the array to be implicitly cast to an immutable slice.
    #[Inline, Overload]
    mtd const opCast()->const [..] T
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
        Debug.assert(.buffer != null)
        return @mkslice(.buffer + lower, (upper + 1) - lower)
    }

    // Returns a constant reference to the element at the specified index.
    #[Inline, Overload]
    mtd const opIndex(index: u64)->const &T
    {
        Debug.safetyBoundCheck(index < .count, "index out of range")
        Debug.assert(.buffer != null)
        return .buffer + index
    }

    // Returns a mutable reference to the element at the specified index.
    #[Inline, Overload]
    mtd opIndex(index: u64)->&T
    {
        Debug.safetyBoundCheck(index < .count, "index out of range")
        Debug.assert(.buffer != null)
        return .buffer + index
    }

    // Assigns a new value to the element at the specified index.
    #[Inline]
    mtd opIndexAffect(index: u64, value: T)
        where Reflection.canCopy(T)
    {
        Debug.safetyBoundCheck(index < .count, "index out of range")
        Debug.assert(.buffer != null)
        .buffer[index] = value
    }

    // Handles compound assignment operators (e.g., `+=`, `-=`) for elements.
    #[Inline]
    mtd(op: string) opIndexAssign(index: u64, value: T)
    {
        Debug.safetyBoundCheck(index < .count, "index out of range")
        #ast "me.buffer[index] " ++ op ++ " value"
    }

    // Replaces the content of the array with elements copied from a slice.
    mtd opAffect(arr: const [..] T)
        where Reflection.canCopy(T)
    {
        .clear()
        var countof = @countof(arr)
        if !countof do
            return
        var dataof = @dataof(arr)

        .grow(countof)
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
    // Visiting by address and in reverse order is supported via parameters.
    // Aliases: #alias0 for the element or a pointer to it, #alias1 for the index.
    #[Macro]
    func(ptr: bool, back: bool) opVisit(const me, stmt: #code void)
    {
        if !.count do
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work correctly
        #if back do
            var scan = .buffer + .count
        #else do
            var scan = .buffer - 1

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

    // Returns a mutable reference to an element using a single coordinate.
    // This can be used to treat the array as a 1D grid.
    #[Inline]
    mtd ref1(x: u64)->&T
    {
        Debug.safetyBoundCheck(x < .count, "index out of range")
        return .buffer + x
    }

    // Returns a mutable reference to an element using 2D coordinates and a width.
    // This can be used to treat the array as a 2D grid.
    #[Inline]
    mtd ref2(x, y, w: u64)->&T
    {
        Debug.safetyBoundCheck(x + y * w < .count, "index out of range")
        return .buffer + x + y * w
    }

    // Reallocates the internal me.buffer to a new capacity.
    // This is a low-level memory operation that handles moving existing elements.
    mtd realloc(newCapacity: u64)
    {
        if .allocator == null do
            .allocator = @getcontext().allocator

        if newCapacity == 0
        {
            Memory.free(.buffer, .capacity * #sizeof(T), .allocator)
            .buffer   = null
            .capacity = 0
            return
        }

        let newBuffer = cast([*] T) Memory.realloc(.buffer, newCapacity * #sizeof(T), .capacity * #sizeof(T), .allocator)

        // Need to call opPostMove on every existing element if the me.buffer has not been reallocated
        // in place
        #if Reflection.hasPostMove(T)
        {
            if .buffer and newBuffer != .buffer
            {
                @postmove(newBuffer, .count)
            }
        }

        .buffer   = newBuffer
        .capacity = newCapacity
    }

    // Frees the array's memory and resets its state.
    // If elements have destructors, they are called.
    mtd free()
    {
        if !.buffer do
            return

        // Need to drop every structs
        #if Reflection.hasDrop(T)
        {
            @drop(.buffer, .count)
        }

        Memory.free(.buffer, .capacity * #sizeof(T), .allocator)
        .buffer = null
        .count, .capacity = 0
    }

    // Creates a new array that uses the temporary allocator.
    // Useful for short-lived arrays to avoid heap fragmentation.
    #[Inline]
    func(TA) createTemp()->Array'TA
    {
        var result: retval
        result.allocator = @getcontext().tempAllocator
        return result
    }

    // Creates a new byte array of a given size.
    // Often used for raw memory me.buffers.
    #[NotGeneric]
    func createBuffer(size: u64)->Array'u8
    {
        var result: retval
        result.resize(size)
        return result
    }

    // Returns 'true' if the array contains no elements.
    #[Inline]
    mtd const isEmpty() => .count == 0

    // Returns a mutable slice containing all elements of the array.
    #[Inline, Overload]
    mtd toSlice()->[..] T
    {
        return @mkslice(.buffer, .count)
    }

    // Returns an immutable slice containing all elements of the array.
    #[Inline, Overload]
    mtd const toSlice()->const [..] T
    {
        return @mkslice(.buffer, .count)
    }

    // Ensures the array has enough capacity to store at least 'newCount' elements.
    // If the current capacity is insufficient, it will be increased.
    // The element me.count remains unchanged. See also [[reserve]].
    mtd grow(newCount: u64)
    {
        if newCount <= .capacity do
            return
        let newCapacity = Math.max(.capacity * 2, newCount)
        .reserve(newCapacity)
    }

    // Sets the array's capacity to at least 'newCapacity'.
    // This does not change the number of elements in the array.
    mtd reserve(newCapacity: u64)
    {
        if !newCapacity
        {
            .free()
            .buffer = null
            .count, .capacity = 0
            return
        }

        if newCapacity <= .capacity do
            return

        .realloc(newCapacity)
    }

    // Ensures the array contains at least 'newCount' elements.
    // If the current me.count is less than 'newCount', new elements are added and default-initialized.
    mtd growResize(newCount: u64)
    {
        if newCount <= .count do
            return
        .grow(newCount)
        .resize(newCount)
    }

    // Changes the number of elements in the array to 'newCount'.
    // If 'newCount' is greater than the current me.count, new elements are default-initialized.
    // If 'newCount' is smaller, excess elements are dropped.
    mtd resize(newCount: u64)
    {
        if newCount == .count do
            return
        if newCount > .count
        {
            .grow(newCount)
            @init(.buffer + .count, newCount - .count)
            .count = newCount
        }
        else
        {
            @drop(.buffer + newCount, .count - newCount)
            .count = newCount
        }
    }

    // Removes all elements from the array, setting its me.count to 0.
    // This calls the destructor for each element if one exists, but does not deallocate memory.
    mtd clear()
    {
        #if Reflection.hasDrop(T)
        {
            for .count do
                @drop(.buffer + @index)
        }

        .count = 0
    }

    // Fills the entire array with the given value.
    // All existing elements are overwritten.
    mtd fill(value: T)
    {
        for i in .count do
            .buffer[i] = value
    }

    // Adds an element to the end of the array, but only if it's not already present.
    // This is slower than 'add' due to the containment check.
    mtd addOnce(value: T)
        where Reflection.canCopy(T)
    {
        if .contains(value) do
            return
        .grow(.count + 1)
        .buffer[.count] = #nodrop value
        .count += 1
    }

    // Adds a copy of an element to the end of the array.
    #[Overload]
    mtd add(value: T)
        where Reflection.canCopy(T)
    {
        .grow(.count + 1)
        .buffer[.count] = #nodrop value
        .count += 1
    }

    // Moves an element to the end of the array.
    #[Overload]
    mtd add(value: &&T)
    {
        .grow(.count + 1)
        .buffer[.count] = #nodrop #move value
        .count += 1
    }

    // Reserves space for 'num' elements at the end of the array and returns a pointer to the start of the new space.
    // The new memory is not initialized.
    mtd emplaceAddress(num: u32 = 1)->[*] T
    {
        .grow(.count + num)
        .count += num
        return .buffer + .count - num
    }

    // Reserves space for 'num' elements at the end of the array and returns a pointer to the start of the new space.
    // The new memory is default-initialized.
    mtd emplaceInitAddress(num: u32 = 1)->[*] T
    {
        .grow(.count + num)
        .count += num
        var res = .buffer + .count - num
        @init(res, num)
        return res
    }

    // Inserts a copy of a value at a specific index, shifting existing elements to the right.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: T)
        where Reflection.canCopy(T)
    {
        Debug.safetyBoundCheck(index <= .count, "index out of range")
        if index == .count do
            return .add(value)
        .grow(.count + 1)
        .count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = .count - 1; idx > index; idx -= 1 do
                .buffer[idx] = #nodrop #moveraw .buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(.buffer + index + 1, .buffer + index, #sizeof(T) * (.count - 1 - index))
        }

        .buffer[index] = #nodrop value
    }

    // Inserts a moved value at a specific index, shifting existing elements to the right.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: &&T)
    {
        Debug.safetyBoundCheck(index <= .count, "index out of range")
        if index == .count do
            return .add(moveref value)
        .grow(.count + 1)
        .count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = .count - 1; idx > index; idx -= 1 do
                .buffer[idx] = #nodrop #moveraw .buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(.buffer + index + 1, .buffer + index, #sizeof(T) * (.count - 1 - index))
        }

        .buffer[index] = #nodrop #move value
    }

    // Inserts elements from a slice at a specific index, shifting existing elements to the right.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, values: const [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return

        Debug.safetyBoundCheck(index <= .count, "index out of range")
        if index == .count do
            return .add(values)
        .grow(.count + sliceCount)
        .count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = .count - 1; idx >= index + sliceCount; idx -= 1 do
                .buffer[idx] = #nodrop #moveraw .buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(.buffer + index + sliceCount, .buffer + index, #sizeof(T) * (.count - sliceCount - index))
        }

        #if Reflection.hasPostCopy(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1 do
                .buffer[index + idx] = #nodrop values[idx]
        }
        #else
        {
            Memory.copy(.buffer + index, @dataof(values), #sizeof(T) * sliceCount)
        }
    }

    // Moves elements from a slice at a specific index, shifting existing elements to the right.
    // Order is preserved.
    mtd emplaceAt(index: u64, values: [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return

        Debug.safetyBoundCheck(index <= .count, "index out of range")
        if index == .count do
            return .add(values)
        .grow(.count + sliceCount)
        .count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = .count - 1; idx >= index + sliceCount; idx -= 1 do
                .buffer[idx] = #nodrop #moveraw .buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(.buffer + index + @countof(values), .buffer + index, #sizeof(T) * (.count - sliceCount - index))
        }

        #if Reflection.hasPostMove(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1 do
                .buffer[index + idx] = #nodrop #move values[idx]
        }
        #else
        {
            Memory.copy(.buffer + index, @dataof(values), #sizeof(T) * sliceCount)
        }
    }

    // Appends all elements from a slice to the end of the array.
    #[Overload]
    mtd add(values: #null const [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount do
            return

        // Do not add myself if there's not enough room (because we will reallocate)
        if .count + sliceCount > .capacity do
            Debug.assert(@dataof(values) < .buffer or @dataof(values) > .buffer + .capacity)

        .grow(.count + sliceCount)
        var oldCount = .count
        .count += sliceCount

        #if Reflection.hasPostCopy(T)
        {
            var destPtr = .buffer + oldCount
            for sliceCount
            {
                dref destPtr = #nodrop values[@index]
                destPtr += 1
            }
        }
        #else
        {
            Memory.copy(.buffer + oldCount, @dataof(values), sliceCount * #sizeof(T))
        }
    }

    // Swaps two elements in the array by their indices.
    mtd swap(idx0, idx1: u64)
    {
        if idx0 == idx1 do
            return
        Debug.safetyBoundCheck(idx0 < .count, "index out of range")
        Debug.safetyBoundCheck(idx1 < .count, "index out of range")
        swap(.buffer + idx0, .buffer + idx1)
    }

    // Removes a specified number of elements starting from a given index.
    // This is an unstable remove; the order of remaining elements is not preserved.
    #[Overload]
    mtd remove(index, num: u64)
    {
        if !num do
            return
        Debug.safetyBoundCheck(index + num <= .count, "index out of range")

        #if !Reflection.hasPostMove(T) and !Reflection.hasDrop(T)
        {
            Memory.copyOver(.buffer + index, .buffer + index + num, (.count - (index + num)) * #sizeof(T))
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
        Debug.safetyBoundCheck(index < .count, "index out of range")
        .count -= 1
        if index == .count do
            @drop(.buffer + .count)
        else do
            .buffer[index] = #moveraw .buffer[.count]
    }

    // Removes one or more elements at a given index, shifting subsequent elements to the left.
    // This is slower than 'removeAt', but preserves the order of the remaining elements.
    mtd removeAtOrdered(index: u64, numValues: u64 = 1)
    {
        if !numValues do
            return
        Debug.safetyBoundCheck(index + numValues <= .count, "index out of range")
        .count -= numValues

        #if Reflection.hasDrop(T)
        {
            for idx in numValues do
                @drop(.buffer + index + idx)
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
            Memory.copyOver(.buffer + index, .buffer + index + numValues, #sizeof(T) * (.count - index))
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
    #[Overload]
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
    #[Overload]
    mtd sort(cb: func||(T, T)->s32)
    {
        Debug.assert(cb != null)
        Slice.sort(.toSlice(), cb)
    }

    // Returns a copy of the first element of the array.
    // Asserts if the array is empty.
    mtd const front()->T
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
        return .buffer
    }

    // Returns a copy of the last element of the array.
    // Asserts if the array is empty.
    mtd const back()->T
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
        return .buffer + .count - 1
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
        @drop(.buffer + .count - 1)
        .count -= 1
    }
}

impl StrConv.IConvert for Array
{
    mtd impl convert(buf: *ConcatBuffer, convFmt: StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes("[")
        for i in .count
        {
            if i do
                buf.addBytes(", ")
            StrConv.convert'T(buf, .buffer[i], convFmt, strFormat)
        }
        buf.addBytes("]")
    }
}
