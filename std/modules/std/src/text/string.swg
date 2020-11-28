namespace std;
using swag

private const SmallSize = 24'u32
private const SmallSizeInternal = SmallSize - @sizeof(IAllocator)
#assert @sizeof(String) == 40

public enum CharacterSet
{
    Latin1
    Unicode
}

public struct String
{
    buffer:     *u8
    length:     u32
    capacity:   u32
    allocator:  IAllocator
    padding:    [SmallSizeInternal] u8
}

impl String
{
    func free(using self)
    {
        memory.free(buffer, allocator)
    }

    func realloc(using self, newCapacity: u32)
    {
        if allocator == null
            allocator = @getcontext().allocator
        buffer = autocast memory.realloc(buffer, newCapacity, allocator)
    }
}

public impl strconv.IConvert for String
{
    func convert(using self, buf: *ConcatBuffer, fmt: const *strconv.ConvertFormat, strFormat: string)
    {
        buf.addBytes(@mkslice(buffer, length))
    }
}

public impl hash.IHash32 for String
{
    func compute(using self)->u32
    {
        return hash.crc32(@mkslice(buffer, length))
    }
}

public impl String
{
    #[inline]
    func opCount(using self)->u32
    {
        return length
    }

    #[inline]
    func opData(using self)->const *void
    {
        return buffer
    }

    func(T) opCast(using self)->T
    {
        #if @typeof(T) == string
            return cast(string) @mkslice(buffer, length)
        #elif @typeof(T) == const [..] u8
            return @mkslice(buffer, length)
        #else
            #assert(false, "invalid cast from 'String' to '" ~ @typeof(T).name ~ "'")
    }

    func opSlice(using self, lower, upper: u32)->string
    {
        debug.assert(lower <= upper && lower < length && upper < length)
        return cast(string) @mkslice(buffer + lower, (upper + 1) - lower)
    }

    func opAffect(using self, value: string)
    {
        if @dataof(value) == null
        {
            reserve(0)
            return
        }

        newCapacity := @countof(value) + 1
        length = newCapacity - 1
        reserve(newCapacity)
        memory.copy(buffer, @dataof(value), length)
        buffer[length] = 0
    }

    func opEquals(using self, other: string)->bool
    {
        if buffer == @dataof(other) // true if null
            return true
        if length != @countof(other)
            return false
        return memory.compare(buffer, @dataof(other), length) == 0
    }

    func opEquals(using self, other: Self)->bool
    {
        if length != other.length
            return false
        return memory.compare(buffer, other.buffer, length) == 0
    }

    func opCmp(using self, other: Self)->s32
    {
        return compare(other.toString(), utf8.ComparisonType.Latin1)
    }

    func(op: string) opAssign(using self, value: string)
    {
        #if op == "+="
            append(value)
        #else
            #assert(false, "invalid binary operator '" ~ op ~ "'")
    }

    func(op: string) opAssign(using self, value: u8)
    {
        #if op == "+="
            append(value)
        #else
            #assert(false, "invalid binary operator '" ~ op ~ "'")
    }

    func opPostMove(using self)
    {
        if buffer && capacity <= SmallSize
            buffer = autocast &allocator
    }

    func opPostCopy(using self)
    {
        if !buffer
            return

        if capacity <= SmallSize
            buffer = autocast &allocator
        else
        {
            oldBuffer := buffer
            buffer = null
            capacity = 0
            reserve(length + 1)
            memory.copy(buffer, oldBuffer, length + 1)
        }
    }

    func opDrop(using self)
    {
        if buffer && buffer != autocast &allocator
            free()
    }

    func opIndex(using self, index: u32)->u8
    {
        if !length return 0
        return buffer[index]
    }

    // Visit the String utf8 bytes
    // @alias0 will contain the byte or the pointer to the byte
    // @alias1 will contain the byte index
    #[macro]
    func(ptr: bool) opVisitBytes(using self, stmt: code)
    {
        if !length return
        scan := buffer
        loop length
        {
            #macro
            {
                #if ptr
                    var @alias0 = `scan
                #else
                    var @alias0 = :`scan
                var @alias1 = @index

                #[swag.noreturn]
                #mixin `stmt
            }

            scan += 1
        }
    }

    // Visit the String unicode characters
    // @alias0 will contain the character or the pointer to the character
    // @alias1 will contain the byte index of that character within the utf8 sequence
    // @alias2 will contain the character index
    #[macro]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        if !length return

        byteIdx := 0'u32
        charIdx := 0'u32
        count := length
        scan := buffer
        while true
        {
            slice := @mkslice(scan, count)
            var (c, eat) = utf8.firstChar(slice)
            if eat == 0
                break
            #macro
            {
                #if ptr
                    var @alias0 = `scan
                #else
                    var @alias0 = `c
                var @alias1 = `byteIdx
                var @alias2 = `charIdx

                #[swag.noreturn]
                #mixin `stmt
            }

            byteIdx += eat
            charIdx += 1
            scan += eat
            count -= eat
        }
    }

    // Set the length of the String to 0
    func clear(using self)
    {
        length = 0
    }

    // Returns the number of unicode characters in the string
    func countChars(using self)->u32
    {
        return utf8.countChars(@mkslice(buffer, length))
    }

    // Returns true if the String has zero length
    func isEmpty(using self)->bool
    {
        return length == 0
    }

    // Returns true if the String is null (undefined)
    func isNull(using self)->bool
    {
        return buffer == null
    }

    // Returns true if the String is null or empty
    func isNullOrEmpty(using self)->bool
    {
        return buffer == null || length == 0
    }

    // Transform a null string in an empty one
    func ensureNotNull(using self)
    {
        if buffer return
        buffer = autocast &allocator
    }

    // Append a byte to the String
    func append(using self, value: u8)
    {
        grow(length + 2)
        buffer[length] = value
        buffer[length + 1] = 0
        length += 1
    }

    // Append a string to the String
    func append(using self, value: string)
    {
        concatLength := @countof(value)
        if concatLength == 0
            return

        newLength := length + concatLength + 1
        grow(newLength)
        memory.copy(buffer + length, @dataof(value), concatLength)
        length = newLength - 1
        buffer[length] = 0
    }

    // Join a list of strings to make a unique one
    func join(values: string...)->String
    {
        var result: String
        loop values
            result += values[@index]
        return result
    }

    // Join an array of strings to make a unique one
    func join(values: const [..] string)->String
    {
        var result: String
        loop values
            result += values[@index]
        return result
    }

    // Join an list of strings to make a unique one, by using a given 'separator'
    // between them
    func joinWith(separator: string, values: string...)->String
    {
        var result: String
        loop values
        {
            if separator && @index
                result += separator
            result += values[@index]
        }

        return result
    }

    // Join an array of strings to make a unique one, by using a given 'separator'
    // between them
    func joinWith(separator: string, values: const [..] string)->String
    {
        var result: String
        loop values
        {
            if separator && @index
                result += separator
            result += values[@index]
        }

        return result
    }

    // Ensure the String is big enough to store a given amount of bytes
    func grow(using self, wantedCapacity: u32)
    {
        if wantedCapacity <= capacity
            return
        newCapacity := math.max(capacity * 2, wantedCapacity)
        reserve(newCapacity)
    }

    // Reserve room for 'newCapacity' bytes without changing the string length
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
                if buffer != autocast &allocator
                    free()
                buffer = null
                length, capacity = 0
                return
            }

            capacity = SmallSize
            if !buffer
                buffer = autocast &allocator
            return
        }

        // Actual size is fine
        if newCapacity <= capacity
            return

        // We were in small size mode, and we are no more
        if buffer == autocast &allocator
        {
            oldBuffer := buffer
            newAllocator := @getcontext().allocator
            buffer = autocast memory.alloc(newCapacity, newAllocator)
            memory.copy(buffer, oldBuffer, length + 1)
            allocator = newAllocator
        }
        else
        {
            realloc(newCapacity)
        }

        capacity = newCapacity
    }

    // Remove whitespaces at the start of the String
    func trimStart(using self)
    {
        if !length
            return

        ptr := buffer
        while true
        {
            slice := @mkslice(ptr, length)
            (c, eat) := utf8.firstChar(slice)
            if !unicode32.isSpace(c)
                break
            length -= eat
            ptr += eat
        }

        memory.copy(buffer, ptr, length)
        buffer[length] = 0
    }

    // Remove whitespaces at the end of the String
    func trimEnd(using self)
    {
        if !length
            return

        while true
        {
            slice := @mkslice(buffer, length)
            (c, eat) := utf8.lastChar(slice)
            if !unicode32.isSpace(c)
                break
            length -= eat
        }

        buffer[length] = 0
    }

    // Removes all leading and trailing white-space characters from the current String
    func trim(using self)
    {
        trimStart()
        trimEnd()
    }

    // Returns the first Unicode character of the String
    // This function will return 0 if the String is null or empty
    func front(using self)->char
    {
        if !length
            return 0
        if latin1.isAscii(buffer[0])
            return cast(char) buffer[0]
        (c, ?) := utf8.firstChar(@mkslice(buffer, length))
        return c
    }

    // Returns the last Unicode character of the String
    // This function will return 0 if the String is null or empty
    func back(using self)->char
    {
        if !length
            return 0
        if latin1.isAscii(buffer[length - 1])
            return cast(char) buffer[length - 1]
        (c, ?) := utf8.lastChar(@mkslice(buffer, length))
        return c
    }

    // Convert string to a 32 bits character array
    func toCharArray(using self)->Array'char
    {
        return unicode32.fromUtf8(@mkslice(buffer, length))
    }

    // Convert a literal string to a String
    func from(value: string)->String
    {
        var val: retval = value
        return val
    }

    // Returns a string type
    func toString(using self)->string
    {
        return cast(string) @mkslice(buffer, length)
    }

    // Convert the string inplace to upper case.
    func makeUpper(using self, set = CharacterSet.Latin1)
    {
        sl := @mkslice(buffer, length)

        using CharacterSet
        switch set
        {
            case Latin1:
                latin1.makeUpper(sl)

            case Unicode:
                uni32 := unicode32.fromUtf8(toString())
                unicode32.makeUpper(uni32.toSlice())
                reserve(uni32.count * 4)
                length = utf8.fromCharArray(@mkslice(buffer, capacity), uni32.toSlice())
        }
    }

    // Convert the string inplace to lower case.
    func makeLower(using self, set = CharacterSet.Latin1)
    {
        sl := @mkslice(buffer, length)

        using CharacterSet
        switch set
        {
            case Latin1:
                latin1.makeLower(sl)

            case Unicode:
                uni32 := unicode32.fromUtf8(toString())
                unicode32.makeLower(uni32.toSlice())
                reserve(uni32.count * 4)
                length = utf8.fromCharArray(@mkslice(buffer, capacity), uni32.toSlice())
        }
    }

    // Returns a new String in upper case
    func toUpper(using self, set = CharacterSet.Latin1)->String
    {
        var result: retval = :self
        result.makeUpper(set)
        return result
    }

    // Returns a new String in lower case
    func toLower(using self, set = CharacterSet.Latin1)->String
    {
        var result: retval = :self
        result.makeLower(set)
        return result
    }

    // Reports the zero-based index (in bytes) of the first occurrence of the specified Unicode character in this String.
    func indexOf(using self, what: char, startByteIndex = 0'u32) -> {index: u32, success: bool}
    {
        return utf8.indexOf(@mkslice(buffer, length), @[what], startByteIndex)
    }

    // Reports the zero-based index (in bytes) of the first occurrence of the specified Unicode character in this String.
    func indexOf(using self, what: const [..] char, startByteIndex = 0'u32) -> {index: u32, success: bool}
    {
        return utf8.indexOf(@mkslice(buffer, length), what, startByteIndex)
    }

    // Reports the zero-based index (in bytes) of the last occurrence of the specified Unicode character in this String.
    func lastIndexOf(using self, what: char) -> {index: u32, success: bool}
    {
        return utf8.lastIndexOf(@mkslice(buffer, length), @[what])
    }

    // Reports the zero-based index (in bytes) of the last occurrence of the specified Unicode character in this String.
    func lastIndexOf(using self, what: const [..] char) -> {index: u32, success: bool}
    {
        return utf8.lastIndexOf(@mkslice(buffer, length), what)
    }

    // Find the given string, and returns the byte index of it
    func indexOf(using self, what: string, startByteIndex = 0'u32, comparisonType = utf8.ComparisonType.Latin1) -> {index: u32, success: bool}
    {
        return utf8.indexOf(@mkslice(buffer, length), what, startByteIndex, comparisonType)
    }

    // Return true if the string starts with 'str'
    func startsWith(using self, what: string, comparisonType = utf8.ComparisonType.Latin1)->bool
    {
        return utf8.startsWith(@mkslice(buffer, length), what, comparisonType)
    }

    // Return true if the string ends with 'str'
    func endsWith(using self, what: string, comparisonType = utf8.ComparisonType.Latin1)->bool
    {
        return utf8.endsWith(@mkslice(buffer, length), what, comparisonType)
    }

    // Compare with another string
    func compare(using self, other: string, comparisonType = utf8.ComparisonType.Latin1)->s32
    {
        return utf8.compare(@mkslice(buffer, length), other, comparisonType)
    }

    // Compare two strings
    func compare(src, dst: string, comparisonType = utf8.ComparisonType.Latin1)->s32
    {
        return utf8.compare(src, dst, comparisonType)
    }

    // Split string into sub strings, given a character separator
    // Note that this returns an array of native strings, not copies.
    // All strings will be invalid if 'this' String is destroyed
    func split(using self, separator: char) -> Array'string
    {
        return split(@[separator])
    }

    // Split string into sub strings, given an array of character separators
    // Note that this returns an array of native strings, not copies.
    // All strings will be invalid if 'this' String is destroyed
    func split(using self, separators: const [..] char) -> Array'string
    {
        var result: Array'string

        var ptr = cast(const *u8) buffer
        var c: char
        var eat: u32

        startbyteIdx := 0'u32
        count := length
        while true
        {
            // Remove separator at the start
            while true
            {
                slice := @mkslice(ptr, count)
                (c, eat) = utf8.firstChar(slice)
                if eat == 0 || !contains(separators, c)
                    break
                startbyteIdx += eat
                ptr += eat
                count -= eat
            }

            if eat == 0 break // done

            // Add all real characters until we found a separator
            endbyteIdx := startbyteIdx
            while true
            {
                slice := @mkslice(ptr, count)
                (c, eat) = utf8.firstChar(slice)
                if eat == 0 || contains(separators, c)
                    break
                endbyteIdx += eat
                ptr += eat
                count -= eat
            }

            oneString := cast(string) @mkslice(buffer + startbyteIdx, endbyteIdx - startbyteIdx)
            result.add(oneString)

            if eat == 0 break // done
            startbyteIdx = endbyteIdx
        }

        return result
    }
}
