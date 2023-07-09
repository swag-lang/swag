#global namespace StrConv
using Swag

// This function will parse 'buf' and poke the corresponding value to 'addr'
// 'addr' must point to an initialized memory location that can hold type 'type'
//
// Accepted types are :
// - s8,s16,s32,s64,s64,u8,u16,u32,u64,u64,rune,bool,f32,f64
// - 'Core.String'  literal strings are not supported as 'buf' is supposed to be transient
// - enum           with or without 'Swag.EnumFlags' attribute
//
// 'buf' can contain multiple values separated with blanks if 'type' is:
// - a static array of the types above
// - a dynamic 'Core.Array' of the types above
public func pokeValue(addr: *void, type: typeinfo, buf: string)->string throw
{
    var patch = cast(*u8) addr
    var one = Tokenize.eatSpaces(buf)

    // Dynamic Core.Array
    if Reflection.isStruct(type)
    {
        let structType = cast(const *TypeInfoStruct) type
        if structType.fromGeneric == @typeof(Array)
        {
            let valueType = structType.generics[0].pointedType
            while @countof(one) and one[0] != "#"'u8
            {
                let addrVal = Reflection.roomInArray(addr, valueType)
                one = try pokeValue(addrVal, valueType, one)
                one = Tokenize.eatSpaces(one)
            }

            return one
        }
    }

    // Static array
    if Reflection.isArray(type)
    {
        let arrayType = cast(const *TypeInfoArray) type
        var addrVal = cast(^u8) addr
        var cpt = 0
        while @countof(one) and one[0] != "#"'u8
        {
            if cpt == arrayType.totalCount throw "too many values"
            one = try pokeValue(addrVal, arrayType.finalType, one)
            one = Tokenize.eatSpaces(one)
            addrVal += arrayType.finalType.sizeof
            cpt += 1
        }

        return one
    }

    // Enum
    if Reflection.isEnum(type)
    {
        let enumType = cast(const *TypeInfoEnum) type
        if enumType.rawType != string and enumType.sizeof > 8
            throw Format.toString("unsupported enum type '%'", enumType.rawType.name)

        let enumFlags = Reflection.hasAttribute(enumType, EnumFlags)

        loop
        {
            one = Tokenize.eatSpaces(one)
            let id = try parseIdentifier(one)
            let v = Reflection.getEnumValue(type, id)
            if !v throw Format.toString("unknown enum value '%'", id)

            if enumType.rawType == string
            {
                dref cast(*string) patch = dref cast(const *string) v.value
            }
            elif !enumFlags
            {
                switch enumType.sizeof
                {
                case 1: dref cast(*u8)  patch = dref cast(const *u8) v.value
                case 2: dref cast(*u16) patch = dref cast(const *u16) v.value
                case 4: dref cast(*u32) patch = dref cast(const *u32) v.value
                case 8: dref cast(*u64) patch = dref cast(const *u64) v.value
                }
            }
            else
            {
                switch enumType.sizeof
                {
                case 1: dref cast(*u8)  patch |= dref cast(const *u8) v.value
                case 2: dref cast(*u16) patch |= dref cast(const *u16) v.value
                case 4: dref cast(*u32) patch |= dref cast(const *u32) v.value
                case 8: dref cast(*u64) patch |= dref cast(const *u64) v.value
                }
            }

            one = Tokenize.eatCount(one, @countof(id))
            if !enumFlags break

            one = Tokenize.eatSpaces(one)
            if !@countof(one) break
            if one[0] == "#"'u8 break

            if one[0] != "|"'u8
                throw "invalid enum flags format"
            one = Tokenize.eatCount(one, 1)
        }

        let eat = cast(u32) cast(s64) (@dataof(one) - @dataof(buf))
        one = Tokenize.eatCount(buf, cast(u64) eat)
        return one
    }

    // Decode interface
    if Reflection.hasInterface(type, IPokeValue)
    {
        let itf = @mkinterface(addr, type, IPokeValue)
        return try itf.poke(one)
    }

    // Basic types
    var eat = 0'u32
    var ovf = false
    switch type
    {
    case s8:
        var val: s8 = undefined
        (val, eat, ovf) = try parseS8(one)
        if ovf throw ParseErrorOverflow
        dref cast(*s8) patch = val

    case s16:
        var val: s16 = undefined
        (val, eat, ovf) = try parseS16(one)
        if ovf throw ParseErrorOverflow
        dref cast(*s16) patch = val

    case s32:
        var val: s32 = undefined
        (val, eat, ovf) = try parseS32(one)
        if ovf throw ParseErrorOverflow
        dref cast(*s32) patch = val

    case s64, s64:
        var val: s64 = undefined
        (val, eat, ovf) = try parseS64(one)
        if ovf throw ParseErrorOverflow
        dref cast(*s64) patch = val

    case u8:
        var val: u8 = undefined
        (val, eat, ovf) = try parseU8(one)
        if ovf throw ParseErrorOverflow
        dref cast(*u8) patch = val

    case u16:
        var val: u16 = undefined
        (val, eat, ovf) = try parseU16(one)
        if ovf throw ParseErrorOverflow
        dref cast(*u16) patch = val

    case u32:
        var val: u32 = undefined
        (val, eat, ovf) = try parseU32(one)
        if ovf throw ParseErrorOverflow
        dref cast(*u32) patch = val

    case u64, u64:
        var val: u64 = undefined
        (val, eat, ovf) = try parseU64(one)
        if ovf throw ParseErrorOverflow
        dref cast(*u64) patch = val

    case f32:
        var val: f32 = undefined
        (val, eat, ovf) = try parseF32(one)
        if ovf throw ParseErrorOverflow
        dref cast(*f32) patch = val

    case f64:
        var val: f64 = undefined
        (val, eat, ovf) = try parseF64(one)
        if ovf throw ParseErrorOverflow
        dref cast(*f64) patch = val

    case bool:
        var val: bool = undefined
        (val, eat) = try parseBool(one)
        dref cast(*bool) patch = val

    case rune:
        var val: rune = undefined
        (val, eat) = try parseRune(one)
        dref cast(*rune) patch = val

    case String:
        var val: String
        (val, eat) = try parseString(one)
        dref cast(*String) patch = val

    default:
        throw Format.toString("unsupported type '%'", type.name)
    }

    return Tokenize.eatCount(one, cast(u64) eat)
}