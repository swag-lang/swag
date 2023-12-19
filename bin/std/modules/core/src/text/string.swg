#global public
using Swag

const StringSmallSize = 16'u64
#assert @sizeof(String) == 56

enum CharacterSet
{
    Latin1
    Unicode
}

struct String
{
    buffer:     ^u8
    length:     u64
    capacity:   u64
    allocator:  IAllocator
    padding:    [StringSmallSize] u8
}

impl String
{
    #[Inline]
    mtd const opCount()->u64
    {
        return length
    }

    #[Inline]
    mtd const opData()->const ^u8
    {
        return buffer
    }

    #[Implicit, Inline, Overload]
    mtd const opCast()->cstring
    {
        return buffer
    }

    #[Implicit, Inline, Overload]
    mtd const opCast()->string
    {
        return @mkstring(buffer, length)
    }

    #[Implicit, Inline, Overload]
    mtd const opCast()->const [..] u8
    {
        return @mkslice(buffer, length)
    }

    #[Implicit]
    mtd opAffect(value: string)
    {
        if @dataof(value) == null
        {
            length = 0
            return
        }

        let newCapacity = @countof(value) + 1
        length = newCapacity - 1
        grow(newCapacity)
        Memory.copy(buffer, @dataof(value), length)
        buffer[length] = 0
    }

    mtd const opSlice(lower, upper: u64)->string
    {
        Debug.assert(lower <= upper and lower < length and upper < length)
        return @mkstring(buffer + lower, (upper + 1) - lower)
    }

    mtd const opEquals(other: string)->bool
    {
        if buffer == @dataof(other) // true if null
            return true
        if length != @countof(other)
            return false
        return Memory.compare(buffer, @dataof(other), length) == 0
    }

    mtd const opCmp(other: string)->s32
    {
        return Utf8.compare(toString(), other, Utf8.ComparisonType.Latin1)
    }

    mtd(op: string) opIndexAssign(index: u64, value: u8)
    {
        Debug.safetyBoundCheck(index < length, "index out of range")
        #if op == "+="
            buffer[index] +=,over value
        #elif op == "-="
            buffer[index] -=,over value
        #else
            #error "invalid operator '" ++ op ++ "'"
    }

    mtd opIndexAffect(index: u64, value: u8)
    {
        Debug.safetyBoundCheck(index < length, "index out of range")
        buffer[index] = value
    }

    mtd(op: string) opAssign(value: string)
    {
        #if op == "+="
            append(value)
        #else
            #error "invalid operator '" ++ op ++ "'"
    }

    mtd(op: string) opAssign(value: rune)
    {
        #if op == "+="
            append(value)
        #else
            #error "invalid operator '" ++ op ++ "'"
    }

    mtd(op: string) opAssign(value: u8)
    {
        #if op == "+="
            append(value)
        #else
            #error "invalid operator '" ++ op ++ "'"
    }

    mtd opPostMove()
    {
        if buffer and capacity <= StringSmallSize
            buffer = acast &padding[0]
    }

    mtd opPostCopy()
    {
        if !buffer
            return

        if(@getcontext().flags & .PostCopyResetAllocator)
            allocator = null

        if capacity <= StringSmallSize
            buffer = acast &padding[0]
        else
        {
            let oldBuffer = buffer
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

    // Returns the byte at the given index
    mtd const opIndex(index: u64)->u8
    {
        Debug.safetyBoundCheck(index < length, "index out of range")
        return buffer[index]
    }

    // Visit the String runes
    // See [[Utf8.visitRunes]] for aliases
    #[Macro]
    mtd(ptr: bool, back: bool) opVisit,runes(stmt: code)
    {
        #if ptr  #error "visiting runes by pointer is not supported"
        #if back #error "visiting runes in reverse order is not supported"
        Utf8.visitRunes(|#alias0, #alias1, #alias2| toSlice(), stmt)
    }

    // Visit the String utf8 bytes
    // * `#alias0` will contain the byte or the pointer to the byte
    // * `#alias1` will contain the byte index
    #[Macro]
    mtd(ptr: bool, back: bool) opVisit,bytes(stmt: code)
    {
        if !length
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work correctly
        #if back
            var scan = buffer + length
        #else
            var scan = buffer - 1

        loop length
        {
            #if back
                scan -= 1
            #else
                scan += 1

            #macro
            {
                #if #up ptr
                    var #alias0 = #up scan
                #else
                    var #alias0 = dref #up scan
                var #alias1 = #index
                #mixin #up stmt
            }
        }
    }

    // Set the length of the String to 0
    mtd clear()
    {
        length = 0
    }

    // Returns true if the String has zero length
    mtd const isEmpty()->bool
    {
        return length == 0
    }

    // Returns true if the String is null (undefined)
    mtd const isNull()->bool
    {
        return buffer == null
    }

    // Returns true if the String is null or empty
    mtd const isNullOrEmpty()->bool
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
        let whatLength = @countof(what)
        if !whatLength or whatLength > length
            return
        let byLength = @countof(by)

        var idx = Utf8.indexOf(toString(), what, 0, comparisonType)
        while idx != Swag.U64.Max
        {
            reserve(length + byLength + 1)
            Memory.copyOver(buffer + idx + byLength, buffer + idx + whatLength, length - idx + 1)
            Memory.copy(buffer + idx, @dataof(by), byLength)
            length -= whatLength
            length += byLength
            idx += byLength
            if idx == length
                break
            idx = Utf8.indexOf(toString(), what, idx, comparisonType)
        }

        buffer[length] = 0;
    }

    // Get the last byte
    mtd back()->u8
    {
        if !length
            return 0
        return buffer[length - 1]
    }

    // Remove some bytes at the given index
    mtd remove(byteIndex: u64, byteCount: u64 = 1)
    {
        if !byteCount
            return
        Debug.safetyBoundCheck(byteIndex + byteCount <= length, "index out of range")
        Memory.copyOver(buffer + byteIndex, buffer + byteIndex + byteCount, length - (byteIndex + byteCount))
        length -= byteCount
        buffer[length] = 0
    }

    // Remove some bytes at the end
    mtd removeBack(byteCount: u64 = 1)
    {
        if !byteCount
            return
        Debug.safetyBoundCheck(byteCount <= length, "index out of range")
        length -= byteCount
        buffer[length] = 0
    }

    // Insert an ascii byte at the given position
    #[Swag.Overload]
    mtd insert(byteIndex: u64, value: u8)
    {
        if byteIndex == length
        {
            append(value)
            return
        }

        Debug.safetyBoundCheck(byteIndex < length, "index out of range")
        Debug.assert(value != 0)

        grow(length + 2)
        Memory.copyOver(buffer + byteIndex + 1, buffer + byteIndex, length - byteIndex + 1)
        buffer[byteIndex] = value
        length += 1
    }

    // Insert a substring at the given position
    #[Swag.Overload]
    mtd insert(byteIndex: u64, value: string)
    {
        if byteIndex == length
        {
            append(value)
            return
        }

        Debug.safetyBoundCheck(byteIndex < length, "index out of range")
        let countof = @countof(value)
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
    #[Swag.Inline, Swag.Overload]
    mtd append(value: u8)
    {
        grow(length + 2)
        var ptr = cast(*u16) (buffer + length)
        dref ptr = value
        length += 1
    }

    // Append a rune to the String
    #[Swag.Overload]
    mtd append(value: rune)
    {
        Debug.assert(Utf8.isValidRune(value), "invalid rune")
        var tmp: [4] u8
        let eat = Utf8.encodeRune(tmp, value)
        grow(length + eat + 1)
        loop i: eat
            buffer[length + i] = tmp[i]
        length += eat
        buffer[length] = 0
    }

    // Append a string to the String
    #[Swag.Overload]
    mtd append(value: string)
    {
        let concatLength = @countof(value)
        if concatLength == 0
            return

        let newLength = length + concatLength + 1
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
            result += values[#index]
        return result
    }

    // Join an list of strings to make a unique one, by using a given 'separator'
    // between them
    func joinWith(separator: string, values: string...)->String
    {
        var result: String
        loop values
        {
            if separator and #index
                result += separator
            result += values[#index]
        }

        return result
    }

    // Ensure the String is big enough to store a given amount of bytes
    #[Swag.Inline]
    mtd grow(wantedCapacity: u64)
    {
        if wantedCapacity <= capacity
            return
        let newCapacity = Math.max(capacity * 2, wantedCapacity)
        reserve(newCapacity)
    }

    // Reserve room for at least 'newCapacity' bytes
    mtd reserve(newCapacity: u64)
    {
        length = Math.min(length, newCapacity)
        if allocator == null
            allocator = @getcontext().allocator

        // Small size, just be sure with have a valid buffer address
        if newCapacity <= StringSmallSize
        {
            // Free dynamic memory if this is not a small string
            if buffer != acast &padding[0]
                Memory.free(buffer, capacity, allocator)

            // This is a small string again
            capacity = StringSmallSize
            buffer = acast &padding[0]
        }

        // We were in small size mode, and we are no more
        elif buffer == acast &padding[0]
        {
            let oldBuffer = buffer
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

        var ptr = buffer
        loop
        {
            let sl = @mkslice(ptr, length)
            let (c, eat) = Utf8.decodeRune(sl)
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
            let sl = @mkslice(buffer, length)
            let (c, eat) = Utf8.decodeLastRune(sl)
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
    mtd const toRuneArray()->Array'rune
    {
        return Unicode.fromUtf8(@mkslice(buffer, length))
    }

    // Convert a literal string to a String
    func from(value: string)->String
    {
        var val: retval = value
        return val
    }

    // Returns a string type
    #[Swag.Inline]
    mtd const toString()->string
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
        let sl = @mkslice(buffer, length)

        using CharacterSet
        switch set
        {
        case Latin1:
            Core.Latin1.makeUpper(sl)

        case Unicode:
            var uni32 = Core.Unicode.fromUtf8(toString())
            Core.Unicode.makeUpper(uni32.toSlice())
            reserve(uni32.count * 4)
            length = Utf8.fromUnicode(@mkslice(buffer, capacity), uni32.toSlice())
        }
    }

    // Convert the string inplace to lower case.
    mtd makeLower(set = CharacterSet.Latin1)
    {
        let sl = @mkslice(buffer, length)

        using CharacterSet
        switch set
        {
        case Latin1:
            Core.Latin1.makeLower(sl)

        case Unicode:
            var uni32 = Core.Unicode.fromUtf8(toString())
            Core.Unicode.makeLower(uni32.toSlice())
            reserve(uni32.count * 4)
            length = Utf8.fromUnicode(@mkslice(buffer, capacity), uni32.toSlice())
        }
    }

    // Returns a new String in upper case
    #[Swag.Overload]
    mtd toUpper(set = CharacterSet.Latin1)->String
    {
        var result: retval = dref self
        result.makeUpper(set)
        return result
    }

    // Returns a new String in lower case
    #[Swag.Overload]
    func toUpper(str: string, set = CharacterSet.Latin1)->String
    {
        var result: retval = str
        result.makeUpper(set)
        return result
    }

    // Returns a new String in lower case
    #[Swag.Overload]
    mtd toLower(set = CharacterSet.Latin1)->String
    {
        var result: retval = dref self
        result.makeLower(set)
        return result
    }

    // Returns a new String in lower case
    #[Swag.Overload]
    func toLower(str: string, set = CharacterSet.Latin1)->String
    {
        var result: retval = str
        result.makeLower(set)
        return result
    }
}

impl Hash.IHash32 for String
{
    func impl compute(using const self)->u32
    {
        return Hash.Crc32.compute(@mkslice(buffer, length))
    }
}