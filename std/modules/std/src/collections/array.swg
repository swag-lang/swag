namespace std.collections
{
    using swag

    public struct(T) Array
    {
        allocator:  const IAllocator
        buffer:     *T
        count:      u32
        capacity:   u32
    }

    public impl Array
    {
        using std.diagnostics

        func opCount(using self)->u32
        {
            return count
        }

        func opDrop(using self)
        {
            free()
        }

        func opIndex(using self, index: u32)->const &T
        {
            debug.assert(index < count, "index out of range")
            return buffer[index]
        }

        func opIndexAffect(using self, value: T, index: u32)
        {
            debug.assert(index < count, "index out of range")
            buffer[index] = value
        }

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
                            let @alias0 = `scan
                        #else
                            let @alias0 = deref `scan
                        let @alias1 = @index
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
                    loop count
                        newBuffer[@index].opPostMove()
                }
                #elif reflection.hasPostCopy(@typeof(T))
                {
                    loop count
                    {
                        newBuffer[@index].opPostCopy()
                        @drop(buffer + @index)
                    }
                }
                #elif reflection.hasDrop(@typeof(T))
                {
                    loop count
                        @drop(buffer + @index)
                }
            }

            buffer = newBuffer
            capacity = newCapacity
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
            buffer[count] = nodrop move deref value
            count += 1
        }

        // Set the number of elements to 0
        func clear(using self)
        {
            count = 0
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

            buffer[index] = move buffer[count]
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
                buffer[idx] = nodrop move buffer[idx + 1]
                idx += 1
            }
        }

        // Insert a value at the given index.
        // Order is preserved.
        func insertAt(using self, index: u32, value: T)
        {
            debug.assert(index <= count, "index out of range")
            if index == count
                return add(value)
            grow(count + 1)
            count += 1

            for idx := count - 1; idx > index; idx -= 1
                buffer[idx] = nodrop move buffer[idx - 1]

            buffer[index] = nodrop value
        }
    }
}