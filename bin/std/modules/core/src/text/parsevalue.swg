#global namespace StrConv
using Swag, Core.Errors

// This function will parse the string 'buf' and decode the corresponding value in 'addr'.
// 'addr' must point to an initialized memory location that can hold 'type'
//
// Accepted types are :
// * `s8, s16, s32, s64, u8, u16, u32, u64, rune, bool, f32, f64`
// * 'Core.String'  literal strings are not supported as 'buf' is supposed to be transient
// * enum           with or without 'Swag.EnumFlags' attribute
//
// 'buf' can contain multiple values separated with blanks if 'type' is:
// * a static array of the types above
// * a dynamic 'Core.Array' of the types above
public func parseValue(addr: *void, type: typeinfo, buf: string)->string throw
{
    var patch = cast(*u8) addr
    var one   = Tokenize.eatSpaces(buf)

    // Dynamic Core.Array
    let valueType = Reflection.getStructArrayType(type)
    if valueType
    {
        while @countof(one) and one[0] != '#'
        {
            let addrVal = Reflection.appendValueStructArray(addr, valueType)
            one = parseValue(addrVal, valueType, one)
            one = Tokenize.eatSpaces(one)
        }

        return one
    }

    // Static array
    if Reflection.isStaticArray(type)
    {
        let arrayType = cast(const *TypeInfoArray) type
        var addrVal   = cast([*] u8) addr
        var cpt       = 0
        while @countof(one) and one[0] != '#'
        {
            if cpt == arrayType.totalCount do
                throw InvalidFormatError{"too many static array values"}
            one = parseValue(addrVal, arrayType.finalType, one)
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
        if enumType.rawType != string and enumType.sizeof > 8 do
            throw UnsupportedFormatError{Errors.mkString(Format.toString("unsupported enum type [[%]]", enumType.rawType.name))}
        let enumFlags = Reflection.hasAttribute(enumType, EnumFlags)

        for
        {
            one = Tokenize.eatSpaces(one)
            let id = parseIdentifier(one)
            let v  = Reflection.getEnumValue(type, id)
            if !v do
                throw InvalidFormatError{Errors.mkString(Format.toString("unknown enum value [[%]]", id))}

            if enumType.rawType == string
            {
                dref cast(*string) patch = dref cast(const *string) v.value
            }
            elif !enumFlags
            {
                switch enumType.sizeof
                {
                case 1:
                    dref cast(*u8) patch = dref cast(const *u8) v.value
                case 2:
                    dref cast(*u16) patch = dref cast(const *u16) v.value
                case 4:
                    dref cast(*u32) patch = dref cast(const *u32) v.value
                case 8:
                    dref cast(*u64) patch = dref cast(const *u64) v.value
                }
            }
            else
            {
                switch enumType.sizeof
                {
                case 1:
                    dref cast(*u8) patch |= dref cast(const *u8) v.value
                case 2:
                    dref cast(*u16) patch |= dref cast(const *u16) v.value
                case 4:
                    dref cast(*u32) patch |= dref cast(const *u32) v.value
                case 8:
                    dref cast(*u64) patch |= dref cast(const *u64) v.value
                }
            }

            one = Tokenize.eatCount(one, @countof(id))
            if !enumFlags do
                break

            one = Tokenize.eatSpaces(one)
            if !@countof(one) do
                break
            if one[0] == '#' do
                break

            if one[0] != '|' do
                throw InvalidFormatError{"invalid enum flags format"}
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
        return itf.poke(one)
    }

    // Basic types
    var eat = 0'u32
    var ovf = false
    switch type
    {
    case s8:
        var val: s8 = undefined
        (val, eat, ovf) = parseS8(one)
        if ovf do
            throw OverflowError{}
        dref cast(*s8) patch = val

    case s16:
        var val: s16 = undefined
        (val, eat, ovf) = parseS16(one)
        if ovf do
            throw OverflowError{}
        dref cast(*s16) patch = val

    case s32:
        var val: s32 = undefined
        (val, eat, ovf) = parseS32(one)
        if ovf do
            throw OverflowError{}
        dref cast(*s32) patch = val

    case s64, s64:
        var val: s64 = undefined
        (val, eat, ovf) = parseS64(one)
        if ovf do
            throw OverflowError{}
        dref cast(*s64) patch = val

    case u8:
        var val: u8 = undefined
        (val, eat, ovf) = parseU8(one)
        if ovf do
            throw OverflowError{}
        dref cast(*u8) patch = val

    case u16:
        var val: u16 = undefined
        (val, eat, ovf) = parseU16(one)
        if ovf do
            throw OverflowError{}
        dref cast(*u16) patch = val

    case u32:
        var val: u32 = undefined
        (val, eat, ovf) = parseU32(one)
        if ovf do
            throw OverflowError{}
        dref cast(*u32) patch = val

    case u64, u64:
        var val: u64 = undefined
        (val, eat, ovf) = parseU64(one)
        if ovf do
            throw OverflowError{}
        dref cast(*u64) patch = val

    case f32:
        var val: f32 = undefined
        (val, eat, ovf) = parseF32(one)
        if ovf do
            throw OverflowError{}
        dref cast(*f32) patch = val

    case f64:
        var val: f64 = undefined
        (val, eat, ovf) = parseF64(one)
        if ovf do
            throw OverflowError{}
        dref cast(*f64) patch = val

    case bool:
        var val: bool = undefined
        (val, eat) = parseBool(one)
        dref cast(*bool) patch = val

    case rune:
        var val: rune = undefined
        (val, eat) = parseRune(one)
        dref cast(*rune) patch = val

    case String:
        var val: String
        (val, eat) = parseString(one)
        dref cast(*String) patch = val

    default:
        throw UnsupportedFormatError{Errors.mkString(Format.toString("unsupported type [[%]]", type.name))}
    }

    return Tokenize.eatCount(one, cast(u64) eat)
}
