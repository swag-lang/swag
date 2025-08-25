#global public
using Swag

const StringSmallSize = 16'u64
#assert(#sizeof(String) == 56)

enum CharacterSet
{
    Latin1
    Unicode
}

struct String
{
    buffer:        ^u8
    length:        u64
    capacity:      u64
    allocator:     IAllocator
    padding:       [StringSmallSize] u8
}

impl String
{
    #[Inline]
    mtd const opCount()->u64
    {
        return me.length
    }

    #[Inline]
    mtd const opData()->const ^u8
    {
        return me.buffer
    }

    #[Implicit, Inline, Overload]
    mtd const opCast()->cstring
    {
        return me.buffer
    }

    #[Implicit, Inline, Overload]
    mtd const opCast()->string
    {
        return @mkstring(me.buffer, me.length)
    }

    #[Implicit, Inline, Overload]
    mtd const opCast()->const [..] u8
    {
        return @mkslice(me.buffer, me.length)
    }

    #[Implicit]
    mtd opAffect(value: string)
    {
        if @dataof(value) == null
        {
            me.length = 0
            return
        }

        let newCapacity = @countof(value) + 1
        me.length = newCapacity - 1
        me.grow(newCapacity)
        Memory.copy(me.buffer, @dataof(value), me.length)
        me.buffer[me.length] = 0
    }

    mtd const opSlice(lower, upper: u64)->string
    {
        Debug.assert(lower <= upper and lower < me.length and upper < me.length)
        return @mkstring(me.buffer + lower, (upper + 1) - lower)
    }

    mtd const opEquals(other: string)->bool
    {
        if me.buffer == @dataof(other):
            return true
        if me.length != @countof(other):
            return false
        return Memory.compare(me.buffer, @dataof(other), me.length) == 0
    }

    mtd const opCmp(other: string)->s32
    {
        return Utf8.compare(me.toString(), other, Utf8.ComparisonType.Latin1)
    }

    mtd(op: string) opIndexAssign(index: u64, value: u8)
    {
        Debug.safetyBoundCheck(index < length, "index out of range")
        #if op == "+=":
            buffer[index] += #wrap value
        #elif op == "-=":
            buffer[index] -= #wrap value
        #else:
            #error("invalid operator '" ++ op ++ "'")
    }

    mtd opIndexAffect(index: u64, value: u8)
    {
        Debug.safetyBoundCheck(index < me.length, "index out of range")
        me.buffer[index] = value
    }

    #[Overload]
    mtd(op: string) opAssign(value: const [..] string)
    {
        #if op == "+=":
            foreach v in value:
                me.append(v)
        #else:
            #error("invalid operator '" ++ op ++ "'")
    }

    #[Overload]
    mtd(op: string) opAssign(value: const [..] rune)
    {
        #if op == "+=":
            foreach v in value:
                me.append(v)
        #else:
            #error("invalid operator '" ++ op ++ "'")
    }

    #[Overload]
    mtd(op: string) opAssign(value: const [..] u8)
    {
        #if op == "+=":
            foreach v in value:
                me.append(v)
        #else:
            #error("invalid operator '" ++ op ++ "'")
    }

    #[Overload]
    mtd(op: string) opAssign(value: string)
    {
        #if op == "+=":
            me.append(value)
        #else:
            #error("invalid operator '" ++ op ++ "'")
    }

    #[Overload]
    mtd(op: string) opAssign(value: rune)
    {
        #if op == "+=":
            me.append(value)
        #else:
            #error("invalid operator '" ++ op ++ "'")
    }

    #[Overload]
    mtd(op: string) opAssign(value: u8)
    {
        #if op == "+=":
            me.append(value)
        #else:
            #error("invalid operator '" ++ op ++ "'")
    }

    mtd opPostMove()
    {
        if me.buffer and me.capacity <= StringSmallSize:
            me.buffer = cast() &me.padding[0]
    }

    mtd opPostCopy()
    {
        if !me.buffer:
            return

        if me.capacity <= StringSmallSize:
            me.buffer = cast() &me.padding[0]
        else
        {
            let oldBuffer = me.buffer
            me.buffer   = null
            me.capacity = 0
            me.reserve(me.length + 1)
            Memory.copy(me.buffer, oldBuffer, me.length + 1)
        }
    }

    mtd opDrop()
    {
        if me.buffer != cast() &me.padding[0]:
            Memory.free(me.buffer, me.capacity, me.allocator)
    }

    // Returns the byte at the given index
    #[Inline]
    mtd const opIndex(index: u64)->u8
    {
        Debug.safetyBoundCheck(index < me.length, "index out of range")
        return me.buffer[index]
    }

    // Visit the String runes
    // See [[Utf8.visitRunes]] for aliases
    #[Macro]
    mtd(ptr: bool, back: bool) opVisitRunes(stmt: #code void)
    {
        #if ptr:
            #error("visiting runes by pointer is not supported")
        #if back:
            #error("visiting runes in reverse order is not supported")
        Utf8.visitRunes(|#alias0, #alias1, #alias2| toSlice(), stmt)
    }

    // Visit the String utf8 bytes
    // * `#alias0` will contain the byte or the pointer to the byte
    // * `#alias1` will contain the byte index
    #[Macro]
    mtd(ptr: bool, back: bool) opVisitBytes(stmt: #code void)
    {
        if !length:
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work correctly
        #if back:
            var scan = buffer + length
        #else:
            var scan = buffer - 1

        for length
        {
            #if back:
                scan -= 1
            #else:
                scan += 1

            #macro
            {
                #if #up ptr:
                    var #alias0 = #up scan
                #else:
                    var #alias0 = dref #up scan
                var #alias1 = @index
                #inject(#up stmt)
            }
        }
    }

    // Default foreach, by bytes.
    #[Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        opVisitBytes'(ptr, back)(|#alias0, #alias1| stmt)
    }

    // Set the length of the String to 0
    #[Inline]
    mtd clear()
    {
        me.length = 0
    }

    // Returns true if the String has zero length
    #[Inline]
    mtd const isEmpty()->bool
    {
        return me.length == 0
    }

    // Returns true if the String is null (undefined)
    #[Inline]
    mtd const isNull()->bool
    {
        return me.buffer == null
    }

    // Returns true if the String is null or empty
    #[Inline]
    mtd const isNullOrEmpty()->bool
    {
        return me.buffer == null or me.length == 0
    }

    // Transform a null string in an empty one
    mtd ensureNotNull()
    {
        if me.buffer:
            return
        me.buffer = cast() &me.padding[0]
    }

    // Replace all occurences of 'what' with 'by'
    #[Overload]
    mtd replace(what: rune, by: string)
    {
        if !me.length:
            return
        let byLength = @countof(by)

        var idx = Utf8.indexOf(me.toString(), what, 0)
        while idx != Swag.U64.Max
        {
            me.reserve(me.length + byLength + 1)
            Memory.copyOver(me.buffer + idx + byLength, me.buffer + idx + 1, me.length - idx + 1)
            Memory.copy(me.buffer + idx, @dataof(by), byLength)
            me.length -= 1
            me.length += byLength
            idx += byLength
            if idx == me.length:
                break
            idx = Utf8.indexOf(me.toString(), what, idx)
        }

        me.buffer[me.length] = 0
    }

    // Replace all occurences of 'what' with 'by'
    #[Overload]
    mtd replace(what, by: string, comparisonType = Utf8.ComparisonType.Latin1)
    {
        let whatLength = @countof(what)
        if !whatLength or whatLength > me.length:
            return
        let byLength = @countof(by)

        var idx = Utf8.indexOf(me.toString(), what, 0, comparisonType)
        while idx != Swag.U64.Max
        {
            me.reserve(me.length + byLength + 1)
            Memory.copyOver(me.buffer + idx + byLength, me.buffer + idx + whatLength, me.length - idx + 1)
            Memory.copy(me.buffer + idx, @dataof(by), byLength)
            me.length -= whatLength
            me.length += byLength
            idx += byLength
            if idx == me.length:
                break
            idx = Utf8.indexOf(me.toString(), what, idx, comparisonType)
        }

        me.buffer[me.length] = 0
    }

    // Get the last byte
    mtd back()->u8
    {
        if !me.length:
            return 0
        return me.buffer[me.length - 1]
    }

    // Remove some bytes at the given index
    mtd remove(byteIndex: u64, byteCount: u64 = 1)
    {
        if !byteCount:
            return
        Debug.safetyBoundCheck(byteIndex + byteCount <= me.length, "index out of range")
        Memory.copyOver(me.buffer + byteIndex, me.buffer + byteIndex + byteCount, me.length - (byteIndex + byteCount))
        me.length -= byteCount
        me.buffer[me.length] = 0
    }

    // Remove some bytes at the end
    mtd removeBack(byteCount: u64 = 1)
    {
        if !byteCount:
            return
        Debug.safetyBoundCheck(byteCount <= me.length, "index out of range")
        me.length -= byteCount
        me.buffer[me.length] = 0
    }

    // Insert an ascii byte at the given position
    #[Overload]
    mtd insert(byteIndex: u64, value: u8)
    {
        if byteIndex == me.length
        {
            me.append(value)
            return
        }

        Debug.safetyBoundCheck(byteIndex < me.length, "index out of range")
        Debug.assert(value != 0)

        me.grow(me.length + 2)
        Memory.copyOver(me.buffer + byteIndex + 1, me.buffer + byteIndex, me.length - byteIndex + 1)
        me.buffer[byteIndex] = value
        me.length += 1
    }

    // Insert a substring at the given position
    #[Overload]
    mtd insert(byteIndex: u64, value: string)
    {
        if byteIndex == me.length
        {
            me.append(value)
            return
        }

        Debug.safetyBoundCheck(byteIndex < me.length, "index out of range")
        let countof = @countof(value)
        if !countof:
            return

        me.grow(me.length + countof + 1)
        Memory.copyOver(me.buffer + byteIndex + countof, me.buffer + byteIndex, me.length - byteIndex + 1)
        Memory.copy(me.buffer + byteIndex, @dataof(value), countof)
        me.length += countof
    }

    // Append a formatted string
    mtd appendFormat(format: string, values: ...)
    {
        me.append(Format.toString(format, values))
    }

    // Append a byte to the String
    #[Inline, Overload]
    mtd append(value: u8)
    {
        me.grow(me.length + 2)
        var ptr = cast(*u16) (me.buffer + me.length)
        dref ptr = value
        me.length += 1
    }

    // Append a rune to the String
    #[Overload]
    mtd append(value: rune)
    {
        Debug.assert(Utf8.isValidRune(value), "invalid rune")
        var tmp: [4] u8
        let eat  = Utf8.encodeRune(tmp, value)
        me.grow(me.length + eat + 1)
        for i in eat:
            me.buffer[me.length + i] = tmp[i]
        me.length += eat
        me.buffer[me.length] = 0
    }

    // Append a string to the String
    #[Overload]
    mtd append(value: string)
    {
        let concatLength = @countof(value)
        if concatLength == 0:
            return

        let newLength = me.length + concatLength + 1
        me.grow(newLength)
        Memory.copy(me.buffer + me.length, @dataof(value), concatLength)
        me.length            = newLength - 1
        me.buffer[me.length] = 0
    }

    // Join a list of strings to make a unique one
    func join(values: string...)->String
    {
        var result: String
        for values:
            result += values[@index]
        return result
    }

    // Join an list of strings to make a unique one, by using a given 'separator'
    // between them
    func joinWith(separator: string, values: string...)->String
    {
        var result: String
        for values
        {
            if separator and @index:
                result += separator
            result += values[@index]
        }

        return result
    }

    // Ensure the String is big enough to store a given amount of bytes
    #[Inline]
    mtd grow(wantedCapacity: u64)
    {
        if wantedCapacity <= me.capacity:
            return
        let newCapacity = Math.max(me.capacity * 2, wantedCapacity)
        me.reserve(newCapacity)
    }

    // Reserve room for at least 'newCapacity' bytes
    mtd reserve(newCapacity: u64)
    {
        me.length = Math.min(me.length, newCapacity)
        if me.allocator == null:
            me.allocator = @getcontext().allocator

        // Small size, just be sure with have a valid buffer address
        if newCapacity <= StringSmallSize
        {
            // Free dynamic memory if this is not a small string
            if me.buffer and me.buffer != cast() &me.padding[0]
            {
                Memory.copy(&me.padding[0], me.buffer, Math.min(newCapacity, me.length + 1))
                Memory.free(me.buffer, me.capacity, me.allocator)
            }

            // This is a small string again
            me.capacity = StringSmallSize
            me.buffer   = cast() &me.padding[0]
        }

        // We were in small size mode, and we are no more
        elif me.buffer == cast() &me.padding[0]
        {
            let oldBuffer = me.buffer
            me.buffer = cast() Memory.alloc(newCapacity, me.allocator)
            Memory.copy(me.buffer, oldBuffer, Math.min(newCapacity, me.length + 1))
            me.capacity = newCapacity
        }

        // We were in dynamic mode, so just realloc memory
        else
        {
            me.buffer   = cast() Memory.realloc(me.buffer, newCapacity, me.capacity, me.allocator)
            me.capacity = newCapacity
        }
    }

    // Remove whitespaces at the start of the String
    mtd trimStart()
    {
        if !me.length:
            return

        var ptr = me.buffer
        for
        {
            let sl = @mkslice(ptr, me.length)
            let (c, eat) = Utf8.decodeRune(sl)
            if !Unicode.isSpace(c):
                break
            me.length -= eat
            ptr += eat
        }

        Memory.copy(me.buffer, ptr, me.length)
        me.buffer[me.length] = 0
    }

    // Remove whitespaces at the end of the String
    mtd trimEnd()
    {
        if !me.length:
            return

        for
        {
            let sl = @mkslice(me.buffer, me.length)
            let (c, eat) = Utf8.decodeLastRune(sl)
            if !Unicode.isSpace(c):
                break
            me.length -= eat
        }

        me.buffer[me.length] = 0
    }

    // Removes all leading and trailing white-space characters from the current String
    mtd trim()
    {
        me.trimStart()
        me.trimEnd()
    }

    // Convert string to a 32 bits character array
    mtd const toRuneArray()->Array'rune
    {
        return Unicode.fromUtf8(@mkslice(me.buffer, me.length))
    }

    // Convert a literal string to a String
    func from(value: #null string)->String
    {
        var val: retval = value
        return val
    }

    // Returns a string type
    #[Inline]
    mtd const toString()->string
    {
        return @mkstring(me.buffer, me.length)
    }

    // Returns a slice type
    #[Inline, Overload]
    mtd toSlice()->[..] u8
    {
        return @mkslice(me.buffer, me.length)
    }

    // Returns a slice type
    #[Inline, Overload]
    mtd const toSlice()->const [..] u8
    {
        return @mkslice(me.buffer, me.length)
    }

    // Convert the string inplace to upper case.
    mtd makeUpper(set = CharacterSet.Latin1)
    {
        let sl = @mkslice(me.buffer, me.length)
        switch set
        {
        case Latin1:
            Core.Latin1.makeUpper(sl)

        case Unicode:
            var uni32 = Core.Unicode.fromUtf8(me.toString())
            Core.Unicode.makeUpper(uni32.toSlice())
            me.reserve(uni32.count * 4)
            me.length = Utf8.fromUnicode(@mkslice(me.buffer, me.capacity), uni32.toSlice())
        }
    }

    // Convert the string inplace to lower case.
    mtd makeLower(set = CharacterSet.Latin1)
    {
        let sl = @mkslice(me.buffer, me.length)
        switch set
        {
        case Latin1:
            Core.Latin1.makeLower(sl)

        case Unicode:
            var uni32 = Core.Unicode.fromUtf8(me.toString())
            Core.Unicode.makeLower(uni32.toSlice())
            me.reserve(uni32.count * 4)
            me.length = Utf8.fromUnicode(@mkslice(me.buffer, me.capacity), uni32.toSlice())
        }
    }

    // Returns a new String in upper case
    #[Overload]
    mtd toUpper(set = CharacterSet.Latin1)->String
    {
        var result: retval = dref me
        result.makeUpper(set)
        return result
    }

    // Returns a new String in lower case
    #[Overload]
    func toUpper(str: #null string, set = CharacterSet.Latin1)->String
    {
        var result: retval = str
        result.makeUpper(set)
        return result
    }

    // Returns a new String in lower case
    #[Overload]
    mtd toLower(set = CharacterSet.Latin1)->String
    {
        var result: retval = dref me
        result.makeLower(set)
        return result
    }

    // Returns a new String in lower case
    #[Overload]
    func toLower(str: #null string, set = CharacterSet.Latin1)->String
    {
        var result: retval = str
        result.makeLower(set)
        return result
    }

    #[Overload]
    {
        // Returns true if the string contains 'what'
        mtd const contains(what: u8)->bool            => Utf8.contains(me.toSlice(), what)
        mtd const contains(what: rune)->bool          => Utf8.contains(me.toSlice(), what)
        mtd const contains(what: string)->bool        => Utf8.contains(me.toSlice(), what)
        mtd const contains(what: const [..] u8)->bool => Utf8.contains(me.toSlice(), what)

        // Return true if the string starts with 'str'
        mtd const startsWith(str: string, comparisonType = Utf8.ComparisonType.Latin1)->bool => Utf8.startsWith(me.toSlice(), str, comparisonType)

        // Find 'what', and returns the byte index of it
        mtd const indexOf(what: string, startByteIndex = 0'u64, comparisonType = Utf8.ComparisonType.Latin1)->u64 => Utf8.indexOf(me.toSlice(), what, startByteIndex, comparisonType)
        mtd const indexOf(what: rune, startByteIndex = 0'u64)->u64                                                => Utf8.indexOf(me.toSlice(), what, startByteIndex)
    }
}

impl Hash.IHash32 for String
{
    func impl compute(using const me)->u32
    {
        return Hash.Crc32.compute(@mkslice(me.buffer, me.length))
    }
}
