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

    impl Array
    {
        public
        {
            func opCount(using self)->u32
            {
                return count
            }

            func opDrop(using self)
            {
                free(self)
            }

            func opIndex(using self, index: u32)->const *T
            {
                std.diagnostics.debug.assert(index < count, "index out of range")
                return buffer + index
            }

            func opIndexAffect(using self, value: T, index: u32)
            {
                std.diagnostics.debug.assert(index < count, "index out of range")
                buffer[index] = value
            }
        }

        public
        {
            func free(using self)
            {
                if !buffer
                    return

                // Need to drop every structs
                #if reflection.hasMember(@typeof(T), "opDrop")
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

                newBuffer := cast(*T) acast memory.realloc(&allocator, buffer, newCapacity * @sizeof(T))

                // Need to call opPostMove/opPostCopy on every existing element if the buffer has not been reallocated
                // in place
                if buffer && newBuffer != buffer
                {
                    #if reflection.hasMember(@typeof(T), "opPostMove")
                    {
                        loop count
                            newBuffer[@index].opPostMove()
                    }
                    #elif reflection.hasMember(@typeof(T), "opPostCopy")
                    {
                        loop count
                        {
                            newBuffer[@index].opPostCopy()
                            @drop(buffer + @index)
                        }
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
                reserve(self, newCapacity)
            }

            // Reserve room for 'newCapacity' elements without changing the array count
            func reserve(using self, newCapacity: u32)
            {
                if !newCapacity
                {
                    free(self)
                    buffer = null
                    count, capacity = 0
                    return
                }

                if newCapacity <= capacity
                    return

                realloc(self, newCapacity)
            }

            // Add a copy of one element at the end of the array
            func add(using self, value: T)
            {
                reserve(self, count + 1)
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
                    self.add(x)
                }
                #else
                {
                    self.add(value)
                }
            }

            // Move one element at the end of the array
            func emplaceBack(using self, value: *T)
            {
                reserve(self, count + 1)
                buffer[count] = nodrop move deref value
                count += 1
            }

            // Set the number of elements to 0
            func clear(using self)
            {
                count = 0
            }

            // Remove an element at 'index' by replacing it with the last
            // element. Order is not preserved
            func removeAt(using self, index: u32)
            {
                std.diagnostics.debug.assert(index < count, "index out of range")
                count -= 1

                if index == count
                {
                    @drop(buffer[count])
                    return
                }

                buffer[index] = move buffer[count]
            }
        }
    }
}