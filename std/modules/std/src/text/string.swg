namespace std.text
{
    using swag
    private const SmallSize = 24'u32
    private const SmallSizeInternal = SmallSize - @sizeof(IAllocator)
    #assert @sizeof(String) == 40

    public readonly struct String
    {
        buffer:     *u8
        length:     u32
        capacity:   u32        
        allocator:  IAllocator

        internal padding: [SmallSizeInternal] u8
    }
    
    impl String
    {
        func free(using self)
        {
            memory.free(&allocator, buffer)
        }

        func realloc(using self, newCapacity: u32)
        {
            if allocator == null
                allocator = @getcontext().allocator
            buffer = acast memory.realloc(&allocator, buffer, newCapacity)
        }      
    }

    impl String
    {
        public
        {
            #[swag.inline]
            #[swag.nodoc]
            func opCount(using self)->u32
            {
                return length
            }

            #[swag.inline]
            #[swag.nodoc]
            func opData(using self)->const *void
            {
                return buffer
            }            

            #[swag.nodoc]
            func(T) opCast(using self)->T
            {
                #if @typeof(T) == @typeof(string)
                    return cast(string) {buffer, length}
                #else
                    #assert(false, "invalid cast from 'String' to '" ~ @typeof(T).name ~ "'")
            }

            #[swag.nodoc]
            func opAffect(using self, value: string)
            {
                if @dataof(value) == null
                {
                    reserve(self, 0)
                    return
                }

                newCapacity := @countof(value) + 1
                length = newCapacity - 1
                reserve(self, newCapacity)
                memory.copy(buffer, @dataof(value), length)
                buffer[length] = 0
            }

            #[swag.nodoc]
            func opEquals(using self, other: string)->bool
            {
                if buffer == @dataof(other)
                    return true
                if length != @countof(other)
                    return false
                return memory.compare(buffer, @dataof(other), length) == 0
            }

            #[swag.nodoc]
            func(op: string) opAssign(using self, value: string)
            {
                #if op == "+="
                    append(self, value)
                #else
                    #assert(false, "invalid binary operator '" ~ op ~ "'")
            }

            #[swag.nodoc]
            func opPostMove(using self)
            {
                if capacity <= SmallSize
                    buffer = acast &allocator
            }

            #[swag.nodoc]
            func opPostCopy(using self)
            {
                if !buffer
                    return
                if capacity <= SmallSize
                    buffer = acast &allocator
                else
                {
                    oldBuffer := buffer
                    buffer = null
                    reserve(self, length + 1)
                    memory.copy(buffer, oldBuffer, length + 1)
                }
            }

            #[swag.nodoc]
            func opDrop(using self)
            {
                if buffer && buffer != acast &allocator
                    free(self)
            }

            /// Visit the [String] utf8 bytes
            #[swag.macro, swag.noreturn]
            func(ptr: bool) opVisitBytes(using self, stmt: code)
            {
                if length
                {
                    scan := buffer
                    loop length
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

            /// Visit the [String] unicode characters
            #[swag.macro, swag.noreturn]
            func(ptr: bool) opVisit(using self, stmt: code)
            {
                if length
                {
                    idx := 0
                    scan := buffer
                    while true
                    {
                        var (nextScan, c) = utf8.nextChar(scan)
                        if c == 0
                            break
                        #macro
                        {
                            #if ptr
                                let @alias0 = `scan
                            #else
                                let @alias0 = `c
                            let @alias1 = `idx
                            #mixin `stmt
                        }

                        idx += cast(s32) (nextScan - scan)
                        scan = cast(*u8) nextScan
                    }
                }
            }             
        }
    }

    impl String
    {
        public
        {
            /// Set the length of the [String] to 0
            func clear(using self)
            {
                length = 0
            }

            /// Returns true if the [String] has zero length
            func isEmpty(using self)->bool
            {
                return length == 0
            }

            /// Returns true if the [String] is null (undefined)
            func isNull(using self)->bool
            {
                return buffer == null
            }

            /// Returns true if the [String] is null or empty
            func isNullOrEmpty(using self)->bool
            {
                return buffer == null || length == 0
            }

            /// Append a string to the [String]
            func append(using self, value: string)
            {
                concatLength := @countof(value)
                if concatLength == 0
                    return

                newLength := length + concatLength + 1
                ensureCapacity(self, newLength)
                memory.copy(buffer + length, @dataof(value), concatLength)
                length = newLength - 1
                buffer[length] = 0
            }

            /// Ensure the [String] is big enough to store a given amount of bytes
            func ensureCapacity(using self, wantedCapacity: u32)
            {
                if wantedCapacity <= capacity
                    return
                newCapacity := math.max(capacity * 2, wantedCapacity)
                reserve(self, newCapacity)
            }

            /// Reserve room for `newCapacity` bytes without changing the string length
            func reserve(using self, newCapacity: u32)
            {
                // Small size, just be sure with have a valid buffer address
                if newCapacity <= SmallSize
                {
                    // Free everything
                    if !newCapacity
                    {
                        if !buffer
                            return
                        if buffer != acast &allocator
                            free(self)
                        buffer = null
                        length, capacity = 0
                        return
                    }

                    capacity = SmallSize
                    if !buffer
                        buffer = acast &allocator
                    return
                }

                // Actual size is fine
                if newCapacity <= capacity
                    return

                // We were in small size mode, and we are no more
                if buffer == acast &allocator
                {
                    oldBuffer := buffer
                    newAllocator := @getcontext().allocator
                    buffer = acast memory.alloc(&newAllocator, newCapacity)
                    memory.copy(buffer, oldBuffer, length + 1)
                    allocator = newAllocator
                }
                else
                {                    
                    realloc(self, newCapacity)
                }
                
                capacity = newCapacity
            }

            /// Remove whitespaces at the start of the [String]
            func trimStart(using self)
            {
                if !length
                    return                

                ptr := cast(const *u8) buffer
                while true
                {
                    var (nextPtr, c) = utf8.nextChar(ptr)
                    if !unicode.isWhiteSpace(c)
                        break
                    length -= cast(u32) (nextPtr - ptr)
                    ptr = nextPtr
                }

                memory.copy(buffer, ptr, length)
                buffer[length] = 0
            }

            /// Remove whitespaces at the end of the [String]
            func trimEnd(using self)
            {
                if !length
                    return   

                ptr := cast(const *u8) buffer + length
                while true
                {
                    var (previousPtr, c) = utf8.previousChar(buffer, ptr)
                    if !unicode.isWhiteSpace(c)
                        break
                    length -= cast(u32) (ptr - previousPtr)
                    ptr = previousPtr
                }

                buffer[length] = 0
            }     

            /// Removes all leading and trailing white-space characters from the current [String]
            ///
            /// The `trim` function removes from the current string all leading and trailing white-space characters. Each leading and trailing 
            /// trim operation stops when a non-white-space character is encountered. For example, if the current string is " abc xyz ", the `trim`
            /// function returns "abc xyz".
            ///
            /// White-space characters are defined by the Unicode standard. The `trim` function removes any leading and trailing characters 
            /// that produce a return value of true when they are passed to the [std.text.unicode.isWhiteSpace] function.
            ///
            /// # Examples
            /// ```
            /// let str: String = "  ABC  "
            /// str.trim()
            /// @assert(str == "ABC")
            /// ```
            ///
            /// # See also
            /// * [std.text.String.trimStart]
            /// * [std.text.String.trimEnd]
            func trim(using self)       
            {
                trimStart(self)
                trimEnd(self)
            }

            /// Reports the zero-based index (in bytes) of the first occurrence of the specified Unicode character in this [String].
            ///
            /// # Remarks
            /// The function will return -1 if the `value` cannot be found
            ///
            /// # Examples
            /// ```
            /// let str: String = "ABC"
            /// @assert(str.indexOf("A"'char) == 0)
            /// @assert(str.indexOf("B"'char) == 1)
            /// @assert(str.indexOf("C"'char) == 2)
            /// @assert(str.indexOf("D"'char) == -1)
            /// ```
            /// The returned index is a byte index, not the index of the unicode character
            /// ```
            /// var str: String = "A⻨C"
            /// @assert(str.indexOf("A"'char) == 0)
            /// @assert(str.indexOf("⻨"'char) == 1)
            /// @assert(str.indexOf("C"'char) == 4)
            /// @assert(str.indexOf("D"'char) == -1)
            /// ```
            func indexOf(using self, value: char)->s32
            {
                if !length
                    return -1 

                visit c, i: deref self
                {
                    if c == value
                        return i
                }

                return -1
            }

            /// Returns the first Unicode character of the [String]
            ///
            /// # Remarks
            /// This function will return 0 if the [String] is null or empty
            ///
            /// # Examples
            /// ```
            /// let str1: String = "ABC"
            /// @assert(str1.front() == "A"'char)
            /// let str2: String = "⾆BC"
            /// @assert(str2.front() == "⾆"'char)
            /// ```
            ///
            /// # See also
            /// * [std.text.String.back]
            func front(using self)->char
            {
                if !length 
                    return 0
                if latin1.isAscii(buffer[0]) 
                    return cast(char) buffer[0]
                let (b, c) = utf8.nextChar(buffer)
                return c
            }

            /// Returns the last Unicode character of the [String]
            ///
            /// # Remarks
            /// This function will return 0 if the [String] is null or empty
            ///
            /// # Examples
            /// ```
            /// let str1: String = "ABC"
            /// @assert(str1.back() == "C"'char)
            /// let str2: String = "AB⾆"
            /// @assert(str2.back() == "⾆"'char)
            /// ```
            ///
            /// # See also
            /// * [std.text.String.front]
            func back(using self)->char
            {
                if !length 
                    return 0
                if latin1.isAscii(buffer[length - 1]) 
                    return cast(char) buffer[length - 1]
                let (b, c) = utf8.previousChar(buffer, buffer + length)
                return c
            }            
        }
    }
}
