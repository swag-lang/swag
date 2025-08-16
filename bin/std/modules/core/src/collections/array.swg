#global export
using Swag

// A generic, dynamic array that can hold elements of any type.
// It automatically manages memory and grows as needed.
struct(T) Array
{
    allocator:     IAllocator     // The allocator used for memory operations.
    buffer:        ^T             // Pointer to the contiguous memory block holding the elements.
    count:         u64            // The number of valid elements currently in the array.
    capacity:      u64            // The total number of elements that can be stored in the allocated self.buffer.
}

impl Array
{
    // Returns the number of elements in the array.
    #[Inline]
    mtd opCount()->u64
    {
        return self.count
    }

    // Returns a raw, constant pointer to the array's self.buffer.
    #[Inline]
    mtd opData()->const *T
    {
        return self.buffer
    }

    // Ensures the allocated memory is freed when the array goes out of scope.
    mtd opDrop()
    {
        self.free()
    }

    // Handles the deep copy of the array when it is copied.
    // A new self.buffer is allocated and the content is copied over.
    mtd opPostCopy()
    {
        if self.buffer == null:
            return

        let oldBuffer = self.buffer
        self.buffer = null
        self.realloc(self.count)

        #if Reflection.hasPostCopy(T)
        {
            for self.count:
                self.buffer[@index] = #nodrop oldBuffer[@index]
        }
        #else
        {
            Memory.copy(self.buffer, oldBuffer, #sizeof(T) * self.count)
        }
    }

    // Allows the array to be implicitly cast to a mutable slice.
    #[Inline, Overload]
    mtd opCast()->[..] T
    {
        return self.toSlice()
    }

    // Allows the array to be implicitly cast to an immutable slice.
    #[Inline, Overload]
    mtd const opCast()->const [..] T
    {
        return self.toSlice()
    }

    // Creates a slice from a sub-part of the array.
    #[Inline]
    mtd opSlice(lower, upper: u64)->[..] T
    {
        Debug.safetyBoundCheck(lower <= upper, "lower bound greater than upper bound")
        Debug.safetyBoundCheck(lower < self.count, "lower bound out of range")
        Debug.safetyBoundCheck(upper < self.count, "upper bound out of range")
        return @mkslice(self.buffer + lower, (upper + 1) - lower)
    }

    // Returns a constant reference to the element at the specified index.
    #[Inline, Overload]
    mtd const opIndex(index: u64)->const &T
    {
        Debug.safetyBoundCheck(index < self.count, "index out of range")
        return self.buffer + index
    }

    // Returns a mutable reference to the element at the specified index.
    #[Inline, Overload]
    mtd opIndex(index: u64)->&T
    {
        Debug.safetyBoundCheck(index < self.count, "index out of range")
        return self.buffer + index
    }

    // Assigns a new value to the element at the specified index.
    #[Inline]
    mtd opIndexAffect(index: u64, value: T)
        where Reflection.canCopy(T)
    {
        Debug.safetyBoundCheck(index < self.count, "index out of range")
        self.buffer[index] = value
    }

    // Handles compound assignment operators (e.g., `+=`, `-=`) for elements.
    #[Inline]
    mtd(op: string) opIndexAssign(index: u64, value: T)
    {
        Debug.safetyBoundCheck(index < self.count, "index out of range")
        #ast "self.buffer[index] " ++ op ++ " value"
    }

    // Replaces the content of the array with elements copied from a slice.
    mtd opAffect(arr: const [..] T)
        where Reflection.canCopy(T)
    {
        self.clear()
        var countof = @countof(arr)
        if !countof:
            return
        var dataof = @dataof(arr)

        self.grow(countof)
        #if Reflection.hasPostCopy(T)
        {
            for i in countof:
                self.buffer[i] = #nodrop dataof[i]
        }
        #else
        {
            Memory.copy(self.buffer, dataof, #sizeof(T) * countof)
        }

        self.count += countof
    }

    // Provides a way to iterate over each element in the array.
    // This is a macro that accepts a code block to execute for each element.
    // Visiting by address and in reverse order is supported via parameters.
    // Aliases: #alias0 for the element or a pointer to it, #alias1 for the index.
    #[Macro]
    func(ptr: bool, back: bool) opVisit(using const self, stmt: #code void)
    {
        if !count:
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work correctly
        #if back:
            var scan = self.buffer + self.count
        #else:
            var scan = self.buffer - 1

        for self.count
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

    // Returns a mutable reference to an element using a single coordinate.
    // This can be used to treat the array as a 1D grid.
    #[Inline]
    mtd ref1(x: u64)->&T
    {
        Debug.safetyBoundCheck(x < self.count, "index out of range")
        return self.buffer + x
    }

    // Returns a mutable reference to an element using 2D coordinates and a width.
    // This can be used to treat the array as a 2D grid.
    #[Inline]
    mtd ref2(x, y, w: u64)->&T
    {
        Debug.safetyBoundCheck(x + y * w < self.count, "index out of range")
        return self.buffer + x + y * w
    }

    // Reallocates the internal self.buffer to a new capacity.
    // This is a low-level memory operation that handles moving existing elements.
    mtd realloc(newCapacity: u64)
    {
        if self.allocator == null:
            self.allocator = @getcontext().allocator

        if newCapacity == 0
        {
            Memory.free(self.buffer, self.capacity * #sizeof(T), self.allocator)
            self.buffer   = null
            self.capacity = 0
            return
        }

        let newBuffer = cast(^T) Memory.realloc(self.buffer, newCapacity * #sizeof(T), self.capacity * #sizeof(T), self.allocator)

        // Need to call opPostMove on every existing element if the self.buffer has not been reallocated
        // in place
        #if Reflection.hasPostMove(T)
        {
            if self.buffer and newBuffer != self.buffer
            {
                @postmove(newBuffer, self.count)
            }
        }

        self.buffer   = newBuffer
        self.capacity = newCapacity
    }

    // Frees the array's memory and resets its state.
    // If elements have destructors, they are called.
    mtd free()
    {
        if !self.buffer:
            return

        // Need to drop every structs
        #if Reflection.hasDrop(T)
        {
            @drop(self.buffer, self.count)
        }

        Memory.free(self.buffer, self.capacity * #sizeof(T), self.allocator)
        self.buffer = null
        self.count, self.capacity = 0
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
    // Often used for raw memory self.buffers.
    #[NotGeneric]
    func createBuffer(size: u64)->Array'u8
    {
        var result: retval
        result.resize(size)
        return result
    }

    // Returns 'true' if the array contains no elements.
    #[Inline]
    mtd const isEmpty() => self.count == 0

    // Returns a mutable slice containing all elements of the array.
    #[Inline, Overload]
    mtd toSlice()->[..] T
    {
        return @mkslice(self.buffer, self.count)
    }

    // Returns an immutable slice containing all elements of the array.
    #[Inline, Overload]
    mtd const toSlice()->const [..] T
    {
        return @mkslice(self.buffer, self.count)
    }

    // Ensures the array has enough capacity to store at least 'newCount' elements.
    // If the current capacity is insufficient, it will be increased.
    // The element self.count remains unchanged. See also [[reserve]].
    mtd grow(newCount: u64)
    {
        if newCount <= self.capacity:
            return
        let newCapacity = Math.max(self.capacity * 2, newCount)
        self.reserve(newCapacity)
    }

    // Sets the array's capacity to at least 'newCapacity'.
    // This does not change the number of elements in the array.
    mtd reserve(newCapacity: u64)
    {
        if !newCapacity
        {
            self.free()
            self.buffer = null
            self.count, self.capacity = 0
            return
        }

        if newCapacity <= self.capacity:
            return

        self.realloc(newCapacity)
    }

    // Ensures the array contains at least 'newCount' elements.
    // If the current self.count is less than 'newCount', new elements are added and default-initialized.
    mtd growResize(newCount: u64)
    {
        if newCount <= self.count:
            return
        self.grow(newCount)
        self.resize(newCount)
    }

    // Changes the number of elements in the array to 'newCount'.
    // If 'newCount' is greater than the current self.count, new elements are default-initialized.
    // If 'newCount' is smaller, excess elements are dropped.
    mtd resize(newCount: u64)
    {
        if newCount == self.count:
            return
        if newCount > self.count
        {
            self.grow(newCount)
            @init(self.buffer + self.count, newCount - self.count)
            self.count = newCount
        }
        else
        {
            @drop(self.buffer + newCount, self.count - newCount)
            self.count = newCount
        }
    }

    // Removes all elements from the array, setting its self.count to 0.
    // This calls the destructor for each element if one exists, but does not deallocate memory.
    mtd clear()
    {
        #if Reflection.hasDrop(T)
        {
            for self.count:
                @drop(self.buffer + @index)
        }

        self.count = 0
    }

    // Fills the entire array with the given value.
    // All existing elements are overwritten.
    mtd fill(value: T)
    {
        for i in self.count:
            self.buffer[i] = value
    }

    // Adds an element to the end of the array, but only if it's not already present.
    // This is slower than 'add' due to the containment check.
    mtd addOnce(value: T)
        where Reflection.canCopy(T)
    {
        if self.contains(value):
            return
        self.grow(self.count + 1)
        self.buffer[self.count] = #nodrop value
        self.count += 1
    }

    // Adds a copy of an element to the end of the array.
    #[Overload]
    mtd add(value: T)
        where Reflection.canCopy(T)
    {
        self.grow(self.count + 1)
        self.buffer[self.count] = #nodrop value
        self.count += 1
    }

    // Moves an element to the end of the array.
    #[Overload]
    mtd add(value: &&T)
    {
        self.grow(self.count + 1)
        self.buffer[self.count] = #nodrop #move value
        self.count += 1
    }

    // Reserves space for 'num' elements at the end of the array and returns a pointer to the start of the new space.
    // The new memory is not initialized.
    mtd emplaceAddress(num: u32 = 1)->^T
    {
        self.grow(self.count + num)
        self.count += num
        return self.buffer + self.count - num
    }

    // Reserves space for 'num' elements at the end of the array and returns a pointer to the start of the new space.
    // The new memory is default-initialized.
    mtd emplaceInitAddress(num: u32 = 1)->^T
    {
        self.grow(self.count + num)
        self.count += num
        var res = self.buffer + self.count - num
        @init(res, num)
        return res
    }

    // Inserts a copy of a value at a specific index, shifting existing elements to the right.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: T)
        where Reflection.canCopy(T)
    {
        Debug.safetyBoundCheck(index <= self.count, "index out of range")
        if index == self.count:
            return self.add(value)
        self.grow(self.count + 1)
        self.count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = self.count - 1; idx > index; idx -= 1:
                self.buffer[idx] = #nodrop #moveraw self.buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(self.buffer + index + 1, self.buffer + index, #sizeof(T) * (self.count - 1 - index))
        }

        self.buffer[index] = #nodrop value
    }

    // Inserts a moved value at a specific index, shifting existing elements to the right.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, value: &&T)
    {
        Debug.safetyBoundCheck(index <= self.count, "index out of range")
        if index == self.count:
            return self.add(moveref value)
        self.grow(self.count + 1)
        self.count += 1

        #if Reflection.hasPostMove(T)
        {
            for var idx = self.count - 1; idx > index; idx -= 1:
                self.buffer[idx] = #nodrop #moveraw self.buffer[idx - 1]
        }
        #else
        {
            Memory.copyOver(self.buffer + index + 1, self.buffer + index, #sizeof(T) * (self.count - 1 - index))
        }

        self.buffer[index] = #nodrop #move value
    }

    // Inserts elements from a slice at a specific index, shifting existing elements to the right.
    // Order is preserved.
    #[Overload]
    mtd insertAt(index: u64, values: const [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount:
            return

        Debug.safetyBoundCheck(index <= self.count, "index out of range")
        if index == self.count:
            return self.add(values)
        self.grow(self.count + sliceCount)
        self.count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = self.count - 1; idx >= index + sliceCount; idx -= 1:
                self.buffer[idx] = #nodrop #moveraw self.buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(self.buffer + index + sliceCount, self.buffer + index, #sizeof(T) * (self.count - sliceCount - index))
        }

        #if Reflection.hasPostCopy(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1:
                self.buffer[index + idx] = #nodrop values[idx]
        }
        #else
        {
            Memory.copy(self.buffer + index, @dataof(values), #sizeof(T) * sliceCount)
        }
    }

    // Moves elements from a slice at a specific index, shifting existing elements to the right.
    // Order is preserved.
    mtd emplaceAt(index: u64, values: [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount:
            return

        Debug.safetyBoundCheck(index <= self.count, "index out of range")
        if index == self.count:
            return self.add(values)
        self.grow(self.count + sliceCount)
        self.count += sliceCount

        #if Reflection.hasPostMove(T)
        {
            for var idx = self.count - 1; idx >= index + sliceCount; idx -= 1:
                self.buffer[idx] = #nodrop #moveraw self.buffer[idx - sliceCount]
        }
        #else
        {
            Memory.copyOver(self.buffer + index + @countof(values), self.buffer + index, #sizeof(T) * (self.count - sliceCount - index))
        }

        #if Reflection.hasPostMove(T)
        {
            for var idx = 0; idx < sliceCount; idx += 1:
                self.buffer[index + idx] = #nodrop #move values[idx]
        }
        #else
        {
            Memory.copy(self.buffer + index, @dataof(values), #sizeof(T) * sliceCount)
        }
    }

    // Appends all elements from a slice to the end of the array.
    #[Overload]
    mtd add(values: #null const [..] T)
        where Reflection.canCopy(T)
    {
        var sliceCount = @countof(values)
        if !sliceCount:
            return

        // Do not add myself if there's not enough room (because we will reallocate)
        if self.count + sliceCount > self.capacity:
            Debug.assert(@dataof(values) < self.buffer or @dataof(values) > self.buffer + self.capacity)

        self.grow(self.count + sliceCount)
        var oldCount = self.count
        self.count += sliceCount

        #if Reflection.hasPostCopy(T)
        {
            var destPtr = self.buffer + oldCount
            for sliceCount
            {
                dref destPtr = #nodrop values[@index]
                destPtr += 1
            }
        }
        #else
        {
            Memory.copy(self.buffer + oldCount, @dataof(values), sliceCount * #sizeof(T))
        }
    }

    // Swaps two elements in the array by their indices.
    mtd swap(idx0, idx1: u64)
    {
        if idx0 == idx1:
            return
        Debug.safetyBoundCheck(idx0 < self.count, "index out of range")
        Debug.safetyBoundCheck(idx1 < self.count, "index out of range")
        swap(self.buffer + idx0, self.buffer + idx1)
    }

    // Removes a specified number of elements starting from a given index.
    // This is an unstable remove; the order of remaining elements is not preserved.
    #[Overload]
    mtd remove(index, num: u64)
    {
        if !num:
            return
        Debug.safetyBoundCheck(index + num <= self.count, "index out of range")

        #if !Reflection.hasPostMove(T) and !Reflection.hasDrop(T)
        {
            Memory.copyOver(self.buffer + index, self.buffer + index + num, (self.count - (index + num)) * #sizeof(T))
        }
        #else
        {
            for i in self.count - num
            {
                self.buffer[index + i] = #moveraw self.buffer[index + num + i]
            }
        }

        self.count -= num
    }

    // Removes the first occurrence of a given value from the array.
    // This is an unstable remove; order is not preserved. Does nothing if the value is not found.
    #[Overload]
    mtd(V) remove(value: V)
    {
        let idx = Slice.findLinear(self.toSlice(), value)
        if idx == Swag.U64.Max:
            return
        self.removeAt(idx)
    }

    // Removes the element at a given index by swapping it with the last element.
    // This is very fast, but the order of elements is not preserved.
    mtd removeAt(index: u64)
    {
        Debug.safetyBoundCheck(index < self.count, "index out of range")
        self.count -= 1
        if index == self.count:
            @drop(self.buffer + self.count)
        else:
            self.buffer[index] = #moveraw self.buffer[self.count]
    }

    // Removes one or more elements at a given index, shifting subsequent elements to the left.
    // This is slower than 'removeAt', but preserves the order of the remaining elements.
    mtd removeAtOrdered(index: u64, numValues: u64 = 1)
    {
        if !numValues:
            return
        Debug.safetyBoundCheck(index + numValues <= self.count, "index out of range")
        self.count -= numValues

        #if Reflection.hasDrop(T)
        {
            for idx in numValues:
                @drop(self.buffer + index + idx)
        }

        if index >= self.count:
            return

        #if Reflection.hasPostMove(T)
        {
            var idx = index
            for self.count - index
            {
                self.buffer[idx] = #nodrop #moveraw self.buffer[idx + numValues]
                idx += 1
            }
        }
        #else
        {
            Memory.copyOver(self.buffer + index, self.buffer + index + numValues, #sizeof(T) * (self.count - index))
        }
    }

    // Removes the first occurrence of a given value, preserving the order of remaining elements.
    // Does nothing if the value is not found.
    mtd(V) removeOrdered(value: V)
    {
        let idx = Slice.findLinear(self.toSlice(), value)
        if idx == Swag.U64.Max:
            return
        self.removeAtOrdered(idx)
    }

    // Returns 'true' if the array contains the given value.
    mtd(V) contains(value: V)->bool
    {
        return Slice.findLinear(self.toSlice(), value) != Swag.U64.Max
    }

    // Sorts the array in ascending order using the default comparison operator.
    #[Overload]
    mtd sort()
    {
        Slice.sort(self.toSlice(), func(x, y: T)->s32 => x <=> y)
    }

    // Sorts the array in descending order.
    mtd sortReverse()
    {
        Slice.sort(self.toSlice(), func(x, y: T)->s32 => y <=> x)
    }

    // Sorts the array using a custom comparison function.
    #[Overload]
    mtd sort(cb: func||(T, T)->s32)
    {
        Debug.assert(cb != null)
        Slice.sort(self.toSlice(), cb)
    }

    // Returns a copy of the first element of the array.
    // Asserts if the array is empty.
    mtd front()->T
    {
        Debug.assert(self.count > 0, "array is empty")
        let result: retval = self.buffer[0]
        return result
    }

    // Returns a pointer to the first element of the array.
    // Asserts if the array is empty.
    mtd frontPtr()->*T
    {
        Debug.assert(self.count > 0, "array is empty")
        return self.buffer
    }

    // Returns a copy of the last element of the array.
    // Asserts if the array is empty.
    mtd back()->T
    {
        Debug.assert(self.count > 0, "array is empty")
        let result: retval = self.buffer[self.count - 1]
        return result
    }

    // Returns a pointer to the last element of the array.
    // Asserts if the array is empty.
    mtd backPtr()->*T
    {
        Debug.assert(self.count > 0, "array is empty")
        return self.buffer + self.count - 1
    }

    // Removes the last element from the array and returns it.
    // Asserts if the array is empty.
    mtd popBack()->T
    {
        Debug.assert(self.count > 0, "array is empty")
        var result: retval = undefined
        result = #nodrop #moveraw self.buffer[self.count - 1]
        self.count -= 1
        return result
    }

    // Removes the last element from the array.
    // Asserts if the array is empty.
    mtd removeBack()
    {
        Debug.assert(self.count > 0, "array is empty")
        @drop(self.buffer + self.count - 1)
        self.count -= 1
    }
}

impl StrConv.IConvert for Array
{
    mtd impl convert(buf: *ConcatBuffer, convFmt: StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes("[")
        for i in self.count
        {
            if i:
                buf.addBytes(", ")
            StrConv.convert'T(buf, self.buffer[i], convFmt, strFormat)
        }
        buf.addBytes("]")
    }
}
