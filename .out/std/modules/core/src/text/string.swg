#global public
using swag

private const SmallSize = 16'uint
#assert @sizeof(String) == 56

enum CharacterSet
{
    Latin1
    Unicode
}

struct String
{
    buffer:     *u8
    length:     uint
    capacity:   uint
    allocator:  IAllocator
    padding:    [SmallSize] u8
}

impl String
{
    #[inline]
    func opCount(using const self)->uint
    {
        return length
    }

    #[inline]
    func opData(using const self)->const *u8
    {
        return buffer
    }

    #[implicit]
    func opCast(using const self)->string
    {
        return @mkstring(buffer, length)
    }

    #[implicit]
    func opCast(using const self)->const [..] u8
    {
        return @mkslice(buffer, length)
    }

    func opSlice(using const self, lower, upper: uint)->string
    {
        debug.assert(lower <= upper and lower < length and upper < length)
        return @mkstring(buffer + lower, (upper + 1) - lower)
    }

    func opAffect(using self, value: string)
    {
        if @dataof(value) == null
        {
            length = 0
            return
        }

        newCapacity := @countof(value) + 1
        length = newCapacity - 1
        grow(newCapacity)
        memory.copy(buffer, @dataof(value), length)
        buffer[length] = 0
    }

    func opEquals(using const self, other: string)->bool
    {
        if buffer == @dataof(other) // true if null
            return true
        if length != @countof(other)
            return false
        return memory.compare(buffer, @dataof(other), length) == 0
    }

    func opCmp(using const self, other: Self)->s32
    {
        return utf8.compare(toString(), other.toString(), utf8.ComparisonType.Latin1)
    }

    func(op: string) opAssign(using self, value: string)
    {
        #if op == "+="
            append(value)
        #else
            #assert(false, "invalid binary operator '" ~ op ~ "'")
    }

    func(op: string) opAssign(using self, value: rune)
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
        if buffer and capacity <= SmallSize
            buffer = acast &padding[0]
    }

    func opPostCopy(using self)
    {
        if !buffer
            return

        if capacity <= SmallSize
            buffer = acast &padding[0]
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
        if buffer != acast &padding[0]
            memory.free(buffer, capacity, allocator)
    }

    func opIndex(using self, index: uint)->u8
    {
        if !length
            return 0
        return buffer[index]
    }

    // Visit the String utf8 bytes
    // @alias0 will contain the byte or the pointer to the byte
    // @alias1 will contain the byte index
    #[macro]
    func(ptr: bool) opVisitBytes(using self, stmt: code)
    {
        if !length
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work
        // correctly, that's why we increment 'scan' before the use code, and take -1 to start.
        scan := buffer - 1

        loop length
        {
            scan += 1
            #macro
            {
                #if `ptr
                    var @alias0 = `scan
                #else
                    var @alias0 = :`scan
                var @alias1 = @index
                #mixin `stmt
            }
        }
    }

    // Visit the String unicode characters
    // @alias0 will contain the character or the pointer to the character
    // @alias1 will contain the byte index of that character within the utf8 sequence
    // @alias2 will contain the character index
    #[macro]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        if !length
            return

        byteIdx := 0'uint
        charIdx := 0'uint
        count := length
        scan := buffer

        label up
        loop
        {
            sl := @mkslice(scan, count)
            var (c, eat) = utf8.decodeRune(sl)
            if eat == 0
                break

            label down
            #macro
            {
                #if `ptr
                    var @alias0 = `scan
                #else
                    var @alias0 = `c
                var @alias1 = `byteIdx
                var @alias2 = `charIdx
                #mixin `stmt { break = break up; continue = break down; }
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

    // Returns true if the String has zero length
    func isEmpty(using const self)->bool
    {
        return length == 0
    }

    // Returns true if the String is null (undefined)
    func isNull(using const self)->bool
    {
        return buffer == null
    }

    // Returns true if the String is null or empty
    func isNullOrEmpty(using const self)->bool
    {
        return buffer == null or length == 0
    }

    // Transform a null string in an empty one
    func ensureNotNull(using self)
    {
        if buffer
            return
        buffer = acast &padding[0]
    }

    // Replace all occurences of 'what' with 'by'
    func replace(using self, what, by: string, comparisonType = utf8.ComparisonType.Latin1)
    {
        whatLength := @countof(what)
        if !whatLength or whatLength > length
            return
        byLength := @countof(by)
        growSize := byLength > whatLength ? (byLength - whatLength) + 1 : 0

        idx := utf8.indexOf(toString(), what, 0, comparisonType)
        while idx != swag.UInt.Max
        {
            grow(length + growSize)
            memory.copyOver(buffer + idx + byLength, buffer + idx + whatLength, length - idx + 1)
            memory.copy(buffer + idx, @dataof(by), byLength)
            length -= whatLength
            length += byLength
            idx += byLength
            if idx == length break
            idx = utf8.indexOf(toString(), what, idx, comparisonType)
        }
    }

    // Insert a substring at the given position
    func insert(using self, byteIndex: uint, value: string)
    {
        debug.assert(byteIndex < length, "invalid index")
        countof := @countof(value)
        if !countof
            return

        grow(length + countof + 1)
        memory.copyOver(buffer + byteIndex + countof, buffer + byteIndex, length - byteIndex + 1)
        memory.copy(buffer + byteIndex, @dataof(value), countof)
        length += countof + 1
    }

    // Append a byte to the String
    func append(using self, value: u8)
    {
        grow(length + 2)
        buffer[length] = value
        buffer[length + 1] = 0
        length += 1
    }

    // Append a rune to the String
    func append(using self, value: rune)
    {
        debug.assert(utf8.isValidRune(value))
        var tmp: [4] u8
        eat := utf8.encodeRune(tmp, value)
        grow(length + eat + 1)
        loop i: eat
            buffer[length + i] = tmp[i]
        length += eat
        buffer[length + 1] = 0
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

    // Join an list of strings to make a unique one, by using a given 'separator'
    // between them
    func joinWith(separator: string, values: string...)->String
    {
        var result: String
        loop values
        {
            if separator and @index
                result += separator
            result += values[@index]
        }

        return result
    }

    // Ensure the String is big enough to store a given amount of bytes
    func grow(using self, wantedCapacity: uint)
    {
        if wantedCapacity <= capacity
            return
        newCapacity := math.max(capacity * 2, wantedCapacity)
        reserve(newCapacity)
    }

    // Reserve room for at least 'newCapacity' bytes
    func reserve(using self, newCapacity: uint)
    {
        length = math.min(length, newCapacity)
        if allocator == null
            allocator = @getcontext().allocator

        // Small size, just be sure with have a valid buffer address
        if newCapacity <= SmallSize
        {
            // Free dynamic memory if this is not a small string
            if buffer != acast &padding[0]
                memory.free(buffer, capacity, allocator)

            // This is a small string again
            capacity = SmallSize
            buffer = acast &padding[0]
        }

        // We were in small size mode, and we are no more
        else if buffer == acast &padding[0]
        {
            oldBuffer := buffer
            buffer = acast memory.alloc(newCapacity, allocator)
            memory.copy(buffer, oldBuffer, math.min(newCapacity, length + 1))
            capacity = newCapacity
        }

        // We were in dynamic mode, so just realloc memory
        else
        {
            buffer = acast memory.realloc(buffer, newCapacity, capacity, allocator)
            capacity = newCapacity
        }
    }

    // Remove whitespaces at the start of the String
    func trimStart(using self)
    {
        if !length
            return

        ptr := buffer
        loop
        {
            sl := @mkslice(ptr, length)
            (c, eat) := utf8.decodeRune(sl)
            if !unicode.isSpace(c)
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

        loop
        {
            sl := @mkslice(buffer, length)
            (c, eat) := utf8.decodeLastRune(sl)
            if !unicode.isSpace(c)
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

    // Convert string to a 32 bits character array
    func toCharArray(using const self)->Array'rune
    {
        return unicode.fromUtf8(@mkslice(buffer, length))
    }

    // Convert a literal string to a String
    func from(value: const string)->String
    {
        var val: retval = value
        return val
    }

    // Returns a string type
    #[swag.inline]
    func toString(using const self)->string
    {
        return @mkstring(buffer, length)
    }

    // Returns a slice type
    #[swag.inline]
    func toSlice(using self)->[..] u8
    {
        return @mkslice(buffer, length)
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
            uni32 := unicode.fromUtf8(toString())
            unicode.makeUpper(uni32.toSlice())
            reserve(uni32.count * 4)
            length = utf8.fromUnicode(@mkslice(buffer, capacity), uni32.toSlice())
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
            uni32 := unicode.fromUtf8(toString())
            unicode.makeLower(uni32.toSlice())
            reserve(uni32.count * 4)
            length = utf8.fromUnicode(@mkslice(buffer, capacity), uni32.toSlice())
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
    func toUpper(str: string, set = CharacterSet.Latin1)->String
    {
        var result: retval = str
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

    // Returns a new String in lower case
    func toLower(str: string, set = CharacterSet.Latin1)->String
    {
        var result: retval = str
        result.makeLower(set)
        return result
    }
}
