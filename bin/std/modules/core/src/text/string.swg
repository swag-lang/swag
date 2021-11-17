#global public
using Swag

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
    #[Inline]
    mtdc opCount()->uint
    {
        return length
    }

    #[Inline]
    mtdc opData()->const *u8
    {
        return buffer
    }

    #[Implicit]
    mtdc opCast()->cstring
    {
        return buffer
    }

    #[Implicit]
    mtdc opCast()->string
    {
        return @mkstring(buffer, length)
    }

    #[Implicit]
    mtdc opCast()->const [..] u8
    {
        return @mkslice(buffer, length)
    }

    mtdc opSlice(lower, upper: uint)->string
    {
        Debug.assert(lower <= upper and lower < length and upper < length)
        return @mkstring(buffer + lower, (upper + 1) - lower)
    }

    mtd opAffect(value: string)
    {
        if @dataof(value) == null
        {
            length = 0
            return
        }

        newCapacity := @countof(value) + 1
        length = newCapacity - 1
        grow(newCapacity)
        Memory.copy(buffer, @dataof(value), length)
        buffer[length] = 0
    }

    mtdc opEquals(other: string)->bool
    {
        if buffer == @dataof(other) // true if null
            return true
        if length != @countof(other)
            return false
        return Memory.compare(buffer, @dataof(other), length) == 0
    }

    mtdc opCmp(other: Self)->s32
    {
        return Utf8.compare(toString(), other.toString(), Utf8.ComparisonType.Latin1)
    }

    mtd(op: string) opAssign(value: string)
    {
        #if op == "+="
            append(value)
        #else
            #assert(false, "invalid binary operator '" ~ op ~ "'")
    }

    mtd(op: string) opAssign(value: rune)
    {
        #if op == "+="
            append(value)
        #else
            #assert(false, "invalid binary operator '" ~ op ~ "'")
    }

    mtd(op: string) opAssign(value: u8)
    {
        #if op == "+="
            append(value)
        #else
            #assert(false, "invalid binary operator '" ~ op ~ "'")
    }

    mtd opPostMove()
    {
        if buffer and capacity <= SmallSize
            buffer = acast &padding[0]
    }

    mtd opPostCopy()
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
            Memory.copy(buffer, oldBuffer, length + 1)
        }
    }

    mtd opDrop()
    {
        if buffer != acast &padding[0]
            Memory.free(buffer, capacity, allocator)
    }

    mtd opIndex(index: uint)->u8
    {
        if !length
            return 0
        return buffer[index]
    }

    // Visit the String utf8 bytes
    // @alias0 will contain the byte or the pointer to the byte
    // @alias1 will contain the byte index
    #[Macro]
    mtd(ptr: bool) opVisitBytes(stmt: code)
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
                    var @alias0 = dref `scan
                var @alias1 = @index
                #mixin `stmt
            }
        }
    }

    // Visit the String unicode characters
    // @alias0 will contain the character or the pointer to the character
    // @alias1 will contain the byte index of that character within the utf8 sequence
    // @alias2 will contain the character index
    #[Macro]
    mtd(ptr: bool) opVisit(stmt: code)
    {
        if !length
            return

        byteIdx := 0'uint
        charIdx := 0'uint
        count := length
        scan := buffer

        scope up
        loop
        {
            sl := @mkslice(scan, count)
            var (c, eat) = Utf8.decodeRune(sl)
            if eat == 0
                break

            scope down
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
    mtd clear()
    {
        length = 0
    }

    // Returns true if the String has zero length
    mtdc isEmpty()->bool
    {
        return length == 0
    }

    // Returns true if the String is null (undefined)
    mtdc isNull()->bool
    {
        return buffer == null
    }

    // Returns true if the String is null or empty
    mtdc isNullOrEmpty()->bool
    {
        return buffer == null or length == 0
    }

    // Transform a null string in an empty one
    mtd ensureNotNull()
    {
        if buffer
            return
        buffer = acast &padding[0]
    }

    // Replace all occurences of 'what' with 'by'
    mtd replace(what, by: string, comparisonType = Utf8.ComparisonType.Latin1)
    {
        whatLength := @countof(what)
        if !whatLength or whatLength > length
            return
        byLength := @countof(by)
        growSize := byLength > whatLength ? (byLength - whatLength) + 1 : 0

        idx := Utf8.indexOf(toString(), what, 0, comparisonType)
        while idx != Swag.UInt.Max
        {
            grow(length + growSize)
            Memory.copyOver(buffer + idx + byLength, buffer + idx + whatLength, length - idx + 1)
            Memory.copy(buffer + idx, @dataof(by), byLength)
            length -= whatLength
            length += byLength
            idx += byLength
            if idx == length
                break
            idx = Utf8.indexOf(toString(), what, idx, comparisonType)
        }
    }

    // Remove some bytes
    mtd remove(byteIndex, byteCount: uint)
    {
        if !byteCount return
        Debug.assert(byteIndex + byteCount <= length)
        Memory.copyOver(buffer + byteIndex, buffer + byteIndex + byteCount, length - (byteIndex + byteCount))
        length -= byteCount
        buffer[length] = 0
    }

    // Insert a substring at the given position
    mtd insert(byteIndex: uint, value: string)
    {
        if byteIndex == length
        {
            append(value)
            return
        }

        Debug.assert(byteIndex < length, "invalid index")
        countof := @countof(value)
        if !countof
            return

        grow(length + countof + 1)
        Memory.copyOver(buffer + byteIndex + countof, buffer + byteIndex, length - byteIndex + 1)
        Memory.copy(buffer + byteIndex, @dataof(value), countof)
        length += countof
    }

    // Append a formatted string
    mtd appendFormat(format: string, values: ...)
    {
        append(Format.toString(format, values))
    }

    // Append a byte to the String
    mtd append(value: u8)
    {
        grow(length + 2)
        buffer[length] = value
        buffer[length + 1] = 0
        length += 1
    }

    // Append a rune to the String
    mtd append(value: rune)
    {
        Debug.assert(Utf8.isValidRune(value))
        var tmp: [4] u8
        eat := Utf8.encodeRune(tmp, value)
        grow(length + eat + 1)
        loop i: eat
            buffer[length + i] = tmp[i]
        length += eat
        buffer[length + 1] = 0
    }

    // Append a string to the String
    mtd append(value: string)
    {
        concatLength := @countof(value)
        if concatLength == 0
            return

        newLength := length + concatLength + 1
        grow(newLength)
        Memory.copy(buffer + length, @dataof(value), concatLength)
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
    mtd grow(wantedCapacity: uint)
    {
        if wantedCapacity <= capacity
            return
        newCapacity := Math.max(capacity * 2, wantedCapacity)
        reserve(newCapacity)
    }

    // Reserve room for at least 'newCapacity' bytes
    mtd reserve(newCapacity: uint)
    {
        length = Math.min(length, newCapacity)
        if allocator == null
            allocator = @getcontext().allocator

        // Small size, just be sure with have a valid buffer address
        if newCapacity <= SmallSize
        {
            // Free dynamic memory if this is not a small string
            if buffer != acast &padding[0]
                Memory.free(buffer, capacity, allocator)

            // This is a small string again
            capacity = SmallSize
            buffer = acast &padding[0]
        }

        // We were in small size mode, and we are no more
        else if buffer == acast &padding[0]
        {
            oldBuffer := buffer
            buffer = acast Memory.alloc(newCapacity, allocator)
            Memory.copy(buffer, oldBuffer, Math.min(newCapacity, length + 1))
            capacity = newCapacity
        }

        // We were in dynamic mode, so just realloc memory
        else
        {
            buffer = acast Memory.realloc(buffer, newCapacity, capacity, allocator)
            capacity = newCapacity
        }
    }

    // Remove whitespaces at the start of the String
    mtd trimStart()
    {
        if !length
            return

        ptr := buffer
        loop
        {
            sl := @mkslice(ptr, length)
            (c, eat) := Utf8.decodeRune(sl)
            if !Unicode.isSpace(c)
                break
            length -= eat
            ptr += eat
        }

        Memory.copy(buffer, ptr, length)
        buffer[length] = 0
    }

    // Remove whitespaces at the end of the String
    mtd trimEnd()
    {
        if !length
            return

        loop
        {
            sl := @mkslice(buffer, length)
            (c, eat) := Utf8.decodeLastRune(sl)
            if !Unicode.isSpace(c)
                break
            length -= eat
        }

        buffer[length] = 0
    }

    // Removes all leading and trailing white-space characters from the current String
    mtd trim()
    {
        trimStart()
        trimEnd()
    }

    // Convert string to a 32 bits character array
    mtdc toCharArray()->Array'rune
    {
        return Unicode.fromUtf8(@mkslice(buffer, length))
    }

    // Convert a literal string to a String
    func from(value: const string)->String
    {
        var val: retval = value
        return val
    }

    // Returns a string type
    #[Swag.Inline]
    mtdc toString()->string
    {
        return @mkstring(buffer, length)
    }

    // Returns a slice type
    #[Swag.Inline]
    mtd toSlice()->[..] u8
    {
        return @mkslice(buffer, length)
    }

    // Convert the string inplace to upper case.
    mtd makeUpper(set = CharacterSet.Latin1)
    {
        sl := @mkslice(buffer, length)

        using CharacterSet
        switch set
        {
        case Latin1:
            Core.Latin1.makeUpper(sl)

        case Unicode:
            uni32 := Core.Unicode.fromUtf8(toString())
            Core.Unicode.makeUpper(uni32.toSlice())
            reserve(uni32.count * 4)
            length = Utf8.fromUnicode(@mkslice(buffer, capacity), uni32.toSlice())
        }
    }

    // Convert the string inplace to lower case.
    mtd makeLower(set = CharacterSet.Latin1)
    {
        sl := @mkslice(buffer, length)

        using CharacterSet
        switch set
        {
        case Latin1:
            Core.Latin1.makeLower(sl)

        case Unicode:
            uni32 := Core.Unicode.fromUtf8(toString())
            Core.Unicode.makeLower(uni32.toSlice())
            reserve(uni32.count * 4)
            length = Utf8.fromUnicode(@mkslice(buffer, capacity), uni32.toSlice())
        }
    }

    // Returns a new String in upper case
    mtd toUpper(set = CharacterSet.Latin1)->String
    {
        var result: retval = dref self
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
        var result: retval = dref self
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
