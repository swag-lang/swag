namespace std;
using swag

private const SmallSize = 24'u32
private const SmallSizeInternal = SmallSize - @sizeof(IAllocator)
#assert @sizeof(String) == 40

public enum CharacterSet
{
    Ascii
    Latin1
}

public enum StringComparison
{
    Raw
    Ascii
    AsciiIgnoreCase
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
        memory.free(&allocator, buffer)
    }

    func realloc(using self, newCapacity: u32)
    {
        if allocator == null
            allocator = @getcontext().allocator
        buffer = acast memory.realloc(&allocator, buffer, newCapacity)
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
        return compare(other, StringComparison.Ascii)
    }

    func(op: string) opAssign(using self, value: string)
    {
        #if op == "+="
            append(value)
        #else
            #assert(false, "invalid binary operator '" ~ op ~ "'")
    }

    func opPostMove(using self)
    {
        if capacity <= SmallSize
            buffer = acast &allocator
    }

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
            capacity = 0
            reserve(length + 1)
            memory.copy(buffer, oldBuffer, length + 1)
        }
    }

    func opDrop(using self)
    {
        if buffer && buffer != acast &allocator
            free()
    }

    func opIndex(using self, index: u32)->u8
    {
        if !length return 0
        return buffer[index]
    }

    // Visit the String utf8 bytes
    #[macro, noreturn]
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

    // Visit the String unicode characters
    #[macro, noreturn]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        if length
        {
            idx := 0
            scan := cast(const *u8) buffer
            while true
            {
                var (nextScan, c) = utf8.nextChar(scan)
                if c == 0
                    break
                #macro
                {
                    #if ptr
                        var @alias0 = `scan
                    #else
                        var @alias0 = `c
                    var @alias1 = `idx
                    #mixin `stmt
                }

                idx += cast(s32) (nextScan - scan)
                scan = cast(const *u8) nextScan
            }
        }
    }

    // Set the length of the String to 0
    func clear(using self)
    {
        length = 0
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
                if buffer != acast &allocator
                    free()
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
            realloc(newCapacity)
        }

        capacity = newCapacity
    }

    // Remove whitespaces at the start of the String
    func trimStart(using self)
    {
        if !length
            return

        ptr := cast(const *u8) buffer
        while true
        {
            (nextPtr, c) := utf8.nextChar(ptr)
            if !unicode32.isWhiteSpace(c)
                break
            length -= cast(u32) (nextPtr - ptr)
            ptr = nextPtr
        }

        memory.copy(buffer, ptr, length)
        buffer[length] = 0
    }

    // Remove whitespaces at the end of the String
    func trimEnd(using self)
    {
        if !length
            return

        ptr := cast(const *u8) buffer + length
        while true
        {
            (previousPtr, c) := utf8.previousChar(buffer, ptr)
            if !unicode32.isWhiteSpace(c)
                break
            length -= cast(u32) (ptr - previousPtr)
            ptr = previousPtr
        }

        buffer[length] = 0
    }

    // Removes all leading and trailing white-space characters from the current String
    func trim(using self)
    {
        trimStart()
        trimEnd()
    }

    // Reports the zero-based index (in bytes) of the first occurrence of the specified Unicode character in this String.
    // The function will return -1 if the `value` cannot be found
    func indexOf(using self, value: char)->s32
    {
        if !length
            return -1

        visit c, i: :self
        {
            if c == value
                return i
        }

        return -1
    }

    // Returns the first Unicode character of the String
    // This function will return 0 if the String is null or empty
    func front(using self)->char
    {
        if !length
            return 0
        if latin1.isAscii(buffer[0])
            return cast(char) buffer[0]
        (b, c) := utf8.nextChar(buffer)
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
        (b, c) := utf8.previousChar(buffer, buffer + length)
        return c
    }

    // Convert string to a 16 bits unicode array
    func toUnicode16(using self)->Array'u16
    {
        var utf16: Array'u16
        utf16.reserve(length + 1)
        count := unicode16.fromUtf8(@mkslice(utf16.buffer, length), @mkslice(buffer, length))
        utf16.count = count + 1
        utf16[count] = 0
        return utf16
    }

    // Convert a literal string to a String
    func from(value: string)->String
    {
        var val: String = value
        return val
    }

    // Returns a string type
    func cstr(using self)->string
    {
        return cast(string) @mkslice(buffer, length)
    }

    // Convert the string inplace to upper case.
    // Only deal with latin1 (not unicode)
    func makeUpper(using self, set: CharacterSet = CharacterSet.Ascii)
    {
        sl := @mkslice(buffer, length)

        using CharacterSet
        switch set
        {
            case Ascii:
                ascii.makeUpper(sl)
            case Latin1:
                utf8.makeUpperLatin1(sl)
        }
    }

    // Convert the string inplace to lower case.
    // Only deal with latin1 (not unicode)
    func makeLower(using self, set: CharacterSet = CharacterSet.Ascii)
    {
        sl := @mkslice(buffer, length)

        using CharacterSet
        switch set
        {
            case Ascii:
                ascii.makeLower(sl)
            case Latin1:
                utf8.makeLowerLatin1(sl)
        }
    }

    // Returns a new String in upper case
    func toUpper(using self, set: CharacterSet = CharacterSet.Ascii)->String
    {
        result := :self
        result.makeUpper(set)
        return result
    }

    // Returns a new String in lower case
    func toLower(using self, set: CharacterSet = CharacterSet.Ascii)->String
    {
        result := :self
        result.makeLower(set)
        return result
    }

    // Return true if the string starts with 'str'
    func startsWidth(using self, str: string, comparisonType: StringComparison = StringComparison.Raw)->bool
    {
        countof := @countof(str)
        if(countof > length) return false
        return compare(cast(string) @mkslice(buffer, countof), str, comparisonType) == 0
    }

    // Return true if the string ends with 'str'
    func endsWidth(using self, str: string, comparisonType: StringComparison = StringComparison.Raw)->bool
    {
        countof := @countof(str)
        if(countof > length) return false
        return compare(cast(string) @mkslice((buffer + length) - countof, countof), str, comparisonType) == 0
    }

    // Compare two strings
    func compare(src, dst: string, comparisonType: StringComparison = StringComparison.Raw)->s32
    {
        using StringComparison
        #[complete]
        switch comparisonType
        {
        case Raw, Ascii:
            return ascii.compare(src, dst)
        case AsciiIgnoreCase:
            return ascii.compare(src, dst, true)
        }

        return 0
    }

    // Compare with another string
    func compare(using self, other: string, comparisonType: StringComparison = StringComparison.Raw)->s32
    {
        return compare(cast(string) @mkslice(buffer, length), other, comparisonType)
    }

    // Compare with another string
    func compare(using self, other: Self, comparisonType: StringComparison = StringComparison.Raw)->s32
    {
        return compare(cast(string) @mkslice(other.buffer, other.length), comparisonType)
    }
}
