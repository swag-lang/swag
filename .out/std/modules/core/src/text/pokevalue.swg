#global namespace parse
using Swag

// Used to add an element in a 'core.Array' without knowing the type of the element at compile time
// but knowing it at runtime (valueType)
private func roomInArray(addr: *void, type: const *TypeInfoStruct, valueType: typeinfo)->*void
{
    // We will treat the array as a buffer of bytes, just to be able to access the fields
    arr := cast(*Array'u8) addr
    sizeValue := valueType.sizeof

    var valueTypeStruct: const *TypeInfoStruct
    if Reflection.isStruct(valueType) valueTypeStruct = acast valueType

    // If we need to reallocate the buffer, we will also have to deal with
    // structures specials functions
    if arr.count >= arr.capacity
    {
        newCapacity := Math.max(arr.count + 1, arr.capacity * 2)
        newBuffer := Memory.alloc(newCapacity * sizeValue, arr.allocator)
        Memory.clear(newBuffer, newCapacity * sizeValue)
        arr.capacity = newCapacity

        // Move old values to the new buffer
        Memory.copy(newBuffer, arr.buffer, arr.count * sizeValue)
        if Reflection.hasPostMove(valueType)
        {
            buf := newBuffer
            loop arr.count
            {
                valueTypeStruct.opPostMove(buf)
                buf += sizeValue
            }
        }

        Memory.free(arr.buffer, arr.count * sizeValue)
        arr.buffer = newBuffer
    }

    result := arr.buffer + (arr.count * valueType.sizeof)
    arr.count += 1
    if valueTypeStruct valueTypeStruct.opInit(result)

    return result
}

// This function will parse 'buf' and poke the corresponding value to 'addr'
// 'addr' must point to an initialized memory location that can hold type 'type'
//
// Accepted types are :
// - s8,s16,s32,s64,int,u8,u16,u32,u64,uint,char,bool,f32,f64
// - 'core.String'  literal strings are not supported as 'buf' is supposed to be transient
// - enum           with or without 'Swag.enumflags' attribute
//
// 'buf' can contain multiple values separated with blanks if 'type' is:
// - a static array of the types above
// - a dynamic 'core.Array' of the types above
public func pokeValue(addr: *void, type: typeinfo, buf: string)->string throw
{
    patch := cast(*u8) addr
    one := tokenize.eatSpaces(buf)

    // Array
    if Reflection.isStructOfName(type, "Array")
    {
        structType := cast(const *TypeInfoStruct) type
        valueType := structType.generics[0].pointedType
        while @countof(one) and one[0] != "#"'u8
        {
            addrVal := roomInArray(addr, structType, valueType)
            one = try pokeValue(addrVal, valueType, one)
            one = tokenize.eatSpaces(one)
        }

        return one
    }

    // Static array
    if Reflection.isArray(type)
    {
        arrayType := cast(const *TypeInfoArray) type
        addrVal := cast(*u8) addr
        cpt := 0
        while @countof(one) and one[0] != "#"'u8
        {
            if cpt == arrayType.totalCount throw "too many values"
            one = try pokeValue(addrVal, arrayType.finalType, one)
            one = tokenize.eatSpaces(one)
            addrVal += arrayType.finalType.sizeof
            cpt += 1
        }

        return one
    }

    // Enum
    if Reflection.isEnum(type)
    {
        enumType := cast(const *TypeInfoEnum) type
        enumFlags := Reflection.hasAttribute(enumType, "Swag.enumflags")

        loop
        {
            one = tokenize.eatSpaces(one)
            id := try getIdentifier(one)
            v := Reflection.getEnumValue(type, id)
            if !v throw format.toString("unknown enum value '%'", id)

            if enumType.rawType == string
                dref cast(*string) patch = dref cast(const *string) v.value
            else
            {
                switch enumType.sizeof
                {
                case 1:     dref cast(*u8) patch |= dref cast(const *u8) v.value
                case 2:     dref cast(*u16) patch |= dref cast(const *u16) v.value
                case 4:     dref cast(*u32) patch |= dref cast(const *u32) v.value
                case 8:     dref cast(*u64) patch |= dref cast(const *u64) v.value
                default:    throw format.toString("unsupported enum type '%'", enumType.rawType.flatName)
                }
            }

            one = tokenize.eatCount(one, @countof(id))
            if !enumFlags break

            one = tokenize.eatSpaces(one)
            if !@countof(one) break
            if one[0] == "#"'u8 break

            if one[0] != "|"'u8
                throw "invalid enum flags format"
            one = tokenize.eatCount(one, 1)
        }

        eat := cast(u32) cast(int) (@dataof(one) - @dataof(buf))
        one = tokenize.eatCount(buf, cast(uint) eat)
        return one
    }

    // Decode interface
    if Reflection.hasInterface(type, IPokeValue)
    {
        itf := @mkinterface(addr, type, IPokeValue)
        return try itf.poke(one)
    }

    // Basic types
    eat := 0'u32
    switch type
    {
    case s8:
        var val: s8 = ?
        (val, eat) = try getS8(one)
        dref cast(*s8) patch = val

    case s16:
        var val: s16 = ?
        (val, eat) = try getS16(one)
        dref cast(*s16) patch = val

    case s32:
        var val: s32 = ?
        (val, eat) = try getS32(one)
        dref cast(*s32) patch = val

    case s64, int:
        var val: s64 = ?
        (val, eat) = try getS64(one)
        dref cast(*s64) patch = val

    case u8:
        var val: u8 = ?
        (val, eat) = try getU8(one)
        dref cast(*u8) patch = val

    case u16:
        var val: u16 = ?
        (val, eat) = try getU16(one)
        dref cast(*u16) patch = val

    case u32:
        var val: u32 = ?
        (val, eat) = try getU32(one)
        dref cast(*u32) patch = val

    case u64, uint:
        var val: u64 = ?
        (val, eat) = try getU64(one)
        dref cast(*u64) patch = val

    case f32:
        var val: f32 = ?
        (val, eat) = try getF32(one)
        dref cast(*f32) patch = val

    case f64:
        var val: f64 = ?
        (val, eat) = try getF64(one)
        dref cast(*f64) patch = val

    case bool:
        var val: bool = ?
        (val, eat) = try getBool(one)
        dref cast(*bool) patch = val

    case rune:
        var val: rune = ?
        (val, eat) = try getRune(one)
        dref cast(*rune) patch = val

    case String:
        var val: String
        (val, eat) = try getString(one)
        dref cast(*String) patch = val

    default:
        throw format.toString("unsupported type '%'", type.flatName)
    }

    return tokenize.eatCount(one, cast(uint) eat)
}