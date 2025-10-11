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
    buffer:        [*] u8
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
        return .length
    }

    #[Inline]
    mtd const opData()->const [*] u8
    {
        return .buffer
    }

    #[Implicit, Inline, Overload]
    mtd const opCast()->cstring
    {
        return .buffer
    }

    #[Implicit, Inline, Overload]
    mtd const opCast()->string
    {
        return @mkstring(.buffer, .length)
    }

    #[Implicit, Inline, Overload]
    mtd const opCast()->const [..] u8
    {
        return @mkslice(.buffer, .length)
    }

    #[Implicit]
    mtd opAffect(value: string)
    {
        if @dataof(value) == null
        {
            .length = 0
            return
        }

        let newCapacity = @countof(value) + 1
        .length = newCapacity - 1
        .grow(newCapacity)
        Memory.copy(.buffer, @dataof(value), .length)
        .buffer[.length] = 0
    }

    mtd const opSlice(lower, upper: u64)->string
    {
        Debug.assert(lower <= upper and lower < .length and upper < .length)
        return @mkstring(.buffer + lower, (upper + 1) - lower)
    }

    mtd const opEquals(other: string)->bool
    {
        if .buffer == @dataof(other) do
            return true
        if .length != @countof(other) do
            return false
        return Memory.compare(.buffer, @dataof(other), .length) == 0
    }

    mtd const opCmp(other: string)->s32
    {
        return Utf8.compare(.toString(), other, Utf8.ComparisonType.Latin1)
    }

    mtd(op: string) opIndexAssign(index: u64, value: u8)
    {
        Debug.safetyBoundCheck(index < .length, "index out of range")
        #if op == "+=" do
            .buffer[index] += #wrap value
        #elif op == "-=" do
            .buffer[index] -= #wrap value
        #else do
            #error("invalid operator '" ++ op ++ "'")
    }

    mtd opIndexAffect(index: u64, value: u8)
    {
        Debug.safetyBoundCheck(index < .length, "index out of range")
        .buffer[index] = value
    }

    #[Overload]
    mtd(op: string) opAssign(value: const [..] string)
    {
        #if op == "+=" do
            foreach v in value do
                .append(v)
        #else do
            #error("invalid operator '" ++ op ++ "'")
    }

    #[Overload]
    mtd(op: string) opAssign(value: const [..] rune)
    {
        #if op == "+=" do
            foreach v in value do
                .append(v)
        #else do
            #error("invalid operator '" ++ op ++ "'")
    }

    #[Overload]
    mtd(op: string) opAssign(value: const [..] u8)
    {
        #if op == "+=" do
            foreach v in value do
                .append(v)
        #else do
            #error("invalid operator '" ++ op ++ "'")
    }

    #[Overload]
    mtd(op: string) opAssign(value: string)
    {
        #if op == "+=" do
            .append(value)
        #else do
            #error("invalid operator '" ++ op ++ "'")
    }

    #[Overload]
    mtd(op: string) opAssign(value: rune)
    {
        #if op == "+=" do
            .append(value)
        #else do
            #error("invalid operator '" ++ op ++ "'")
    }

    #[Overload]
    mtd(op: string) opAssign(value: u8)
    {
        #if op == "+=" do
            .append(value)
        #else do
            #error("invalid operator '" ++ op ++ "'")
    }

    mtd opPostMove()
    {
        if .buffer and .capacity <= StringSmallSize do
            .buffer = cast() &.padding[0]
    }

    mtd opPostCopy()
    {
        if !.buffer do
            return

        if .capacity <= StringSmallSize do
            .buffer = cast() &.padding[0]
        else
        {
            let oldBuffer = .buffer
            .buffer   = null
            .capacity = 0
            .reserve(.length + 1)
            Memory.copy(.buffer, oldBuffer, .length + 1)
        }
    }

    mtd opDrop()
    {
        if .buffer != cast() &.padding[0] do
            Memory.free(.buffer, .capacity, .allocator)
    }

    // Returns the byte at the given index
    #[Inline]
    mtd const opIndex(index: u64)->u8
    {
        Debug.safetyBoundCheck(index < .length, "index out of range")
        return .buffer[index]
    }

    // Visit the String runes
    // See [[Utf8.visitRunes]] for aliases
    #[Macro]
    mtd(ptr: bool, back: bool) const opVisitRunes(stmt: #code void)
    {
        #if ptr do
            #error("visiting runes by pointer is not supported")
        #if back do
            #error("visiting runes in reverse order is not supported")
        Utf8.visitRunes(|#alias0, #alias1, #alias2| .toSlice(), stmt)
    }

    // Visit the String utf8 bytes
    // * `#alias0` will contain the byte or the pointer to the byte
    // * `#alias1` will contain the byte index
    #[Macro]
    mtd(ptr: bool, back: bool) const opVisitBytes(stmt: #code void)
    {
        if !.length do
            return

        // Little trick to avoid labels. That way a 'continue' in the user code will work correctly
        #if back do
            var scan = .buffer + .length
        #else do
            var scan = .buffer - 1

        for .length
        {
            #if back do
                scan -= 1
            #else do
                scan += 1

            #macro
            {
                #if #up ptr do
                    var #alias0 = #up scan
                #else do
                    var #alias0 = dref #up scan
                var #alias1 = @index
                #inject(#up stmt)
            }
        }
    }

    // Default foreach, by bytes.
    #[Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        .opVisitBytes'(ptr, back)(|#alias0, #alias1| stmt)
    }

    // Set the length of the String to 0
    #[Inline]
    mtd clear()
    {
        .length = 0
    }

    // Returns true if the String has zero length
    #[Inline]
    mtd const isEmpty()->bool
    {
        return .length == 0
    }

    // Returns true if the String is null (undefined)
    #[Inline]
    mtd const isNull()->bool
    {
        return .buffer == null
    }

    // Returns true if the String is null or empty
    #[Inline]
    mtd const isNullOrEmpty()->bool
    {
        return .buffer == null or .length == 0
    }

    // Transform a null string in an empty one
    mtd ensureNotNull()
    {
        if .buffer do
            return
        .buffer = cast() &.padding[0]
    }

    // Replace all occurences of 'what' with 'by'
    #[Overload]
    mtd replace(what: rune, by: string)
    {
        if !.length do
            return
        let byLength = @countof(by)

        var idx = Utf8.indexOf(.toString(), what, 0)
        while idx != Swag.U64.Max
        {
            .reserve(.length + byLength + 1)
            Memory.copyOver(.buffer + idx + byLength, .buffer + idx + 1, .length - idx + 1)
            Memory.copy(.buffer + idx, @dataof(by), byLength)
            .length -= 1
            .length += byLength
            idx += byLength
            if idx == .length do
                break
            idx = Utf8.indexOf(.toString(), what, idx)
        }

        .buffer[.length] = 0
    }

    // Replace all occurences of 'what' with 'by'
    #[Overload]
    mtd replace(what, by: string, comparisonType = Utf8.ComparisonType.Latin1)
    {
        let whatLength = @countof(what)
        if !whatLength or whatLength > .length do
            return
        let byLength = @countof(by)

        var idx = Utf8.indexOf(.toString(), what, 0, comparisonType)
        while idx != Swag.U64.Max
        {
            .reserve(.length + byLength + 1)
            Memory.copyOver(.buffer + idx + byLength, .buffer + idx + whatLength, .length - idx + 1)
            Memory.copy(.buffer + idx, @dataof(by), byLength)
            .length -= whatLength
            .length += byLength
            idx += byLength
            if idx == .length do
                break
            idx = Utf8.indexOf(.toString(), what, idx, comparisonType)
        }

        .buffer[.length] = 0
    }

    // Get the last byte
    mtd back()->u8
    {
        if !.length do
            return 0
        return .buffer[.length - 1]
    }

    // Remove some bytes at the given index
    mtd remove(byteIndex: u64, byteCount: u64 = 1)
    {
        if !byteCount do
            return
        Debug.safetyBoundCheck(byteIndex + byteCount <= .length, "index out of range")
        Memory.copyOver(.buffer + byteIndex, .buffer + byteIndex + byteCount, .length - (byteIndex + byteCount))
        .length -= byteCount
        .buffer[.length] = 0
    }

    // Remove some bytes at the end
    mtd removeBack(byteCount: u64 = 1)
    {
        if !byteCount do
            return
        Debug.safetyBoundCheck(byteCount <= .length, "index out of range")
        .length -= byteCount
        .buffer[.length] = 0
    }

    // Insert an ascii byte at the given position
    #[Overload]
    mtd insert(byteIndex: u64, value: u8)
    {
        if byteIndex == .length
        {
            .append(value)
            return
        }

        Debug.safetyBoundCheck(byteIndex < .length, "index out of range")
        Debug.assert(value != 0)

        .grow(.length + 2)
        Memory.copyOver(.buffer + byteIndex + 1, .buffer + byteIndex, .length - byteIndex + 1)
        .buffer[byteIndex] = value
        .length += 1
    }

    // Insert a substring at the given position
    #[Overload]
    mtd insert(byteIndex: u64, value: string)
    {
        if byteIndex == .length
        {
            .append(value)
            return
        }

        Debug.safetyBoundCheck(byteIndex < .length, "index out of range")
        let countof = @countof(value)
        if !countof do
            return

        .grow(.length + countof + 1)
        Memory.copyOver(.buffer + byteIndex + countof, .buffer + byteIndex, .length - byteIndex + 1)
        Memory.copy(.buffer + byteIndex, @dataof(value), countof)
        .length += countof
    }

    // Append a formatted string
    mtd appendFormat(format: string, values: ...)
    {
        .append(Format.toString(format, values))
    }

    // Append a byte to the String
    #[Inline, Overload]
    mtd append(value: u8)
    {
        .grow(.length + 2)
        var ptr = cast(*u16) (.buffer + .length)
        dref ptr = value
        .length += 1
    }

    // Append a rune to the String
    #[Overload]
    mtd append(value: rune)
    {
        Debug.assert(Utf8.isValidRune(value), "invalid rune")
        var tmp: [4] u8
        let eat  = Utf8.encodeRune(tmp, value)
        .grow(.length + eat + 1)
        for i in eat do
            .buffer[.length + i] = tmp[i]
        .length += eat
        .buffer[.length] = 0
    }

    // Append a string to the String
    #[Overload]
    mtd append(value: string)
    {
        let concatLength = @countof(value)
        if concatLength == 0 do
            return

        let newLength = .length + concatLength + 1
        .grow(newLength)
        Memory.copy(.buffer + .length, @dataof(value), concatLength)
        .length          = newLength - 1
        .buffer[.length] = 0
    }

    // Join a list of strings to make a unique one
    func join(values: string...)->String
    {
        var result: String
        for values do
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
            if separator and @index do
                result += separator
            result += values[@index]
        }

        return result
    }

    // Ensure the String is big enough to store a given amount of bytes
    #[Inline]
    mtd grow(wantedCapacity: u64)
    {
        if wantedCapacity <= .capacity do
            return
        let newCapacity = Math.max(.capacity * 2, wantedCapacity)
        .reserve(newCapacity)
    }

    // Reserve room for at least 'newCapacity' bytes
    mtd reserve(newCapacity: u64)
    {
        .length = Math.min(.length, newCapacity)
        if .allocator == null do
            .allocator = @getcontext().allocator

        // Small size, just be sure with have a valid buffer address
        if newCapacity <= StringSmallSize
        {
            // Free dynamic memory if this is not a small string
            if .buffer and .buffer != cast() &.padding[0]
            {
                Memory.copy(&.padding[0], .buffer, Math.min(newCapacity, .length + 1))
                Memory.free(.buffer, .capacity, .allocator)
            }

            // This is a small string again
            .capacity = StringSmallSize
            .buffer   = cast() &.padding[0]
        }

        // We were in small size mode, and we are no more
        elif .buffer == cast() &.padding[0]
        {
            let oldBuffer = .buffer
            .buffer = cast() Memory.alloc(newCapacity, .allocator)
            Memory.copy(.buffer, oldBuffer, Math.min(newCapacity, .length + 1))
            .capacity = newCapacity
        }

        // We were in dynamic mode, so just realloc memory
        else
        {
            .buffer   = cast() Memory.realloc(.buffer, newCapacity, .capacity, .allocator)
            .capacity = newCapacity
        }
    }

    // Remove whitespaces at the start of the String
    mtd trimStart()
    {
        if !.length do
            return

        var ptr = .buffer
        for
        {
            let sl = @mkslice(ptr, .length)
            let (c, eat) = Utf8.decodeRune(sl)
            if !Unicode.isSpace(c) do
                break
            .length -= eat
            ptr += eat
        }

        Memory.copy(.buffer, ptr, .length)
        .buffer[.length] = 0
    }

    // Remove whitespaces at the end of the String
    mtd trimEnd()
    {
        if !.length do
            return

        for
        {
            let sl = @mkslice(.buffer, .length)
            let (c, eat) = Utf8.decodeLastRune(sl)
            if !Unicode.isSpace(c) do
                break
            .length -= eat
        }

        .buffer[.length] = 0
    }

    // Removes all leading and trailing white-space characters from the current String
    mtd trim()
    {
        .trimStart()
        .trimEnd()
    }

    // Convert string to a 32 bits character array
    mtd const toRuneArray()->Array'rune
    {
        return Unicode.fromUtf8(@mkslice(.buffer, .length))
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
        return @mkstring(.buffer, .length)
    }

    // Returns a slice type
    #[Inline, Overload]
    mtd toSlice()->[..] u8
    {
        return @mkslice(.buffer, .length)
    }

    // Returns a slice type
    #[Inline, Overload]
    mtd const toSlice()->const [..] u8
    {
        return @mkslice(.buffer, .length)
    }

    // Convert the string inplace to upper case.
    mtd makeUpper(set = CharacterSet.Latin1)
    {
        let sl = @mkslice(.buffer, .length)
        switch set
        {
        case Latin1:
            Core.Latin1.makeUpper(sl)

        case Unicode:
            var uni32 = Core.Unicode.fromUtf8(.toString())
            Core.Unicode.makeUpper(uni32.toSlice())
            .reserve(uni32.count * 4)
            .length = Utf8.fromUnicode(@mkslice(.buffer, .capacity), uni32.toSlice())
        }
    }

    // Convert the string inplace to lower case.
    mtd makeLower(set = CharacterSet.Latin1)
    {
        let sl = @mkslice(.buffer, .length)
        switch set
        {
        case Latin1:
            Core.Latin1.makeLower(sl)

        case Unicode:
            var uni32 = Core.Unicode.fromUtf8(.toString())
            Core.Unicode.makeLower(uni32.toSlice())
            .reserve(uni32.count * 4)
            .length = Utf8.fromUnicode(@mkslice(.buffer, .capacity), uni32.toSlice())
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
        mtd const contains(what: u8)->bool            => Utf8.contains(.toSlice(), what)
        mtd const contains(what: rune)->bool          => Utf8.contains(.toSlice(), what)
        mtd const contains(what: string)->bool        => Utf8.contains(.toSlice(), what)
        mtd const contains(what: const [..] u8)->bool => Utf8.contains(.toSlice(), what)

        // Return true if the string starts with 'str'
        mtd const startsWith(str: string, comparisonType = Utf8.ComparisonType.Latin1)->bool => Utf8.startsWith(.toSlice(), str, comparisonType)

        // Find 'what', and returns the byte index of it
        mtd const indexOf(what: string, startByteIndex = 0'u64, comparisonType = Utf8.ComparisonType.Latin1)->u64 => Utf8.indexOf(.toSlice(), what, startByteIndex, comparisonType)
        mtd const indexOf(what: rune, startByteIndex = 0'u64)->u64                                                => Utf8.indexOf(.toSlice(), what, startByteIndex)
    }
}

impl Hash.IHash32 for String
{
    func impl compute(const me)->u32
    {
        return Hash.Crc32.compute(@mkslice(.buffer, .length))
    }
}
