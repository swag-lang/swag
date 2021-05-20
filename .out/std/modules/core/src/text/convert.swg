#global public
#global namespace StrConv
using Swag, Globalization

struct ConvertFormat
{
    value: any
}

// Format structure to convert a float to a string
struct ConvertFormatFloat
{
    using val:  ConvertFormat
    precision:  s8 = -1 // -1 is for 'most possible precision'
    fmt:        u8 = "f"'u8
    forceSign:  bool
}

impl ConvertFormatFloat
{
    // Set some format options with a given user string
    // Format is :
    //
    // [+][fmt][precision]
    //
    // [+]
    //      force the positive sign if number is positive
    // [fmt]
    //      output format:
    //      'e' (-d.dddde±dd, a decimal exponent)
    //      'E' (-d.ddddE±dd, a decimal exponent)
    //      'f' (-ddd.dddd, no exponent)
    //      'g' ('e' for large exponents, 'f' otherwise)
    //      'G' ('E' for large exponents, 'f' otherwise)
    // [precision]
    //      integer, precision of the fractional part
    func setFormat(using self, strFormat: string)
    {
        buffer := @dataof(strFormat)
        countof := @countof(strFormat)
        if buffer == null or countof == 0
            return

        cpt := 0'u32

        // Force sign for positive numbers
        if(buffer[cpt] == "+"'u8)
        {
            forceSign = true
            cpt += 1
        }

        // Format
        if cpt < countof
        {
            switch buffer[cpt]
            {
            case "e"'u8, "E"'u8, "f"'u8, "g"'u8, "G"'u8:
                fmt = buffer[cpt]
                cpt += 1
            }
        }

        // Precision
        if cpt < countof
        {
            (userWidth, eatCount) := assume Parse.getU64(@mkstring(buffer + cpt, cast(uint) (countof - cpt)))
            Debug.assert(userWidth <= 255, "precision overflow")
            if eatCount precision = cast(s8) userWidth
        }
    }
}

// Format structure to convert an integer to a string
// If 'signed' is 'true', the value to convert must be stored in 'signedValue', otherwise it must
// be stored in 'unsignedValue'.
struct ConvertFormatInt
{
    using val:      ConvertFormat
    base:           u32 = 10
    padding:        u8
    width:          u8
    forceSign:      bool
}

impl ConvertFormatInt
{
    // Set some format options with a given user string
    // Format is :
    //
    // [+][B|D|X][padding][width]
    //
    // [+]
    //      force sign for positive numbers
    // [B|D|X]
    //      binary, decimal, hexadecimal
    // [padding]
    //      padding character, in ascii, mandatory before the next field
    // [width]
    //      width of the output, in characters
    func setFormat(using self, strFormat: string)
    {
        buffer := @dataof(strFormat)
        countof := @countof(strFormat)
        if buffer == null or countof == 0
            return

        cpt := 0'u32

        // Force sign for positive numbers
        if(buffer[cpt] == "+"'u8)
        {
            forceSign = true
            cpt += 1
        }

        // Base
        switch buffer[cpt]
        {
            case "B"'u8: base = 2; cpt += 1
            case "D"'u8: base = 10; cpt += 1
            case "X"'u8: base = 16; cpt += 1
        }

        // Next comes the padding characters
        if cpt >= countof
            return
        padding = buffer[cpt]
        cpt += 1

        // Next comes the padding width
        if cpt >= countof
            return
        (userWidth, eatCount) := assume Parse.getU64(@mkstring(buffer + cpt, cast(uint) countof - cpt))
        Debug.assert(userWidth <= 255, "padding width overflow")
        if eatCount width = cast(u8) userWidth
    }
}

// Convert a value to an utf8 string, and append the result in a ConcatBuffer
func convertAny(buf: *ConcatBuffer, value: any, strFormat: string)
{
    type := cast(const *TypeInfo) @kindof(value)

    // This is a type
    if Reflection.isType(type)
    {
        ptrType := dref cast(*typeinfo) @dataof(value)
        buf.addBytes(ptrType.name)
        return
    }

    // A pointer, whatever type
    if Reflection.isPointer(type)
    {
        var fmt: ConvertFormatInt
        fmt.base = 16
        fmt.width = 16
        fmt.padding = "0"'u8
        fmt.setFormat(strFormat)
        fmt.value = dref cast(const *u64) @dataof(value)
        convertInt(buf, fmt)
        return
    }

    // An enum value
    if Reflection.isEnum(type)
    {
        enumName := Reflection.getEnumName(type, @dataof(value))
        if !enumName.isEmpty()
            buf.addBytes(@mkslice(enumName.buffer, cast(uint) enumName.length))
        else
            buf.addBytes("?")
        return
    }

    // A struct
    if Reflection.isStruct(type)
    {
        if type == ConvertFormatInt
            convertInt(buf, cast(ConvertFormatInt) value)
        else if type == ConvertFormatFloat
            convertFloat(buf, cast(ConvertFormatFloat) value)
        else
            convertStruct(buf, value, strFormat)
        return
    }

    // A slice
    if Reflection.isSlice(type)
    {
        ptrSlice := cast(const *TypeInfoSlice) type
        buf.addBytes("[")

        sl := cast(const [..] u8) @dataof(value)
        bufSlice := @dataof(sl)
        loop @countof(sl)
        {
            if @index buf.addBytes(",")
            v := @mkany(bufSlice, ptrSlice.pointedType)
            convertAny(buf, v, strFormat)
            bufSlice += ptrSlice.pointedType.sizeof
        }

        buf.addBytes("]")
        return
    }

    // A static array
    if Reflection.isArray(type)
    {
        ptrArr := cast(const *TypeInfoArray) type
        buf.addBytes("[")

        bufArr := cast(const *u8) @dataof(value)
        loop ptrArr.count
        {
            if @index buf.addBytes(",")
            v := @mkany(bufArr, ptrArr.pointedType)
            convertAny(buf, v, strFormat)
            bufArr += ptrArr.pointedType.sizeof
        }

        buf.addBytes("]")
        return
    }

    if Reflection.isInteger(type)
    {
        convertInt(buf, value, strFormat)
        return
    }

    if Reflection.isFloat(type)
    {
        convertFloat(buf, value, strFormat)
        return
    }

    switch type
    {
        case bool:
            convertBool(buf, cast(bool) value, strFormat)
        case rune:
            convertRune(buf, cast(rune) value, strFormat)
        case string:
            buf.addBytes(cast(string) value)
        default:
            buf.addBytes("?")
    }
}

// Convert a structure content to an utf8 string, and put the result in a ConcatBuffer
func convertStruct(buf: *ConcatBuffer, value: any, strFormat: string)
{
    kindof := @kindof(value)
    Debug.assert(kindof.kind == TypeInfoKind.Struct, "this is not a structure")
    ptr := @dataof(value)

    // A structure with an implementation of IConvert
    itf := @mkinterface(ptr, kindof, IConvert)
    if itf != null
    {
        itf.convert(buf, null, strFormat)
        return
    }

    // Do it field by field
    buf.addBytes("{")

    type := cast(const *TypeInfoStruct) kindof
    visit field: type.fields
    {
        if(@index != 0)
            buf.addBytes(", ")
        buf.addBytes(field.name)
        buf.addBytes(" = ")
        val := @mkany(ptr + field.offset, field.pointedType)
        convertAny(buf, val, strFormat)
    }

    buf.addBytes("}")
}

// Convert a float to an utf8 string, and put the result in a ConcatBuffer
func convertFloat(buf: *ConcatBuffer, fmt: ConvertFormatFloat)
{
    kindof := @kindof(fmt.value)
    Debug.assert(kindof.flags & TypeInfoFlags.Float != 0, "value to convert is not a float")

    dataof := @dataof(fmt.value)
    bits := kindof.sizeof * 8

    var value: f64
    switch kindof
    {
        case f32: value = cast(f64) dref cast(*f32) dataof
        case f64: value = dref cast(*f64) dataof
    }

    dtoa(buf, value, fmt.fmt, cast(s64) fmt.precision, fmt.forceSign)
}

// Convert an integer to an utf8 string, and put the result in a ConcatBuffer
func convertInt(buf: *ConcatBuffer, fmt: ConvertFormatInt)
{
    kindof := @kindof(fmt.value)
    Debug.assert(kindof.flags & TypeInfoFlags.Integer != 0, "value to convert is not an integer")
    Debug.assert(0 < fmt.base and fmt.base <= 16, "invalid convert base")

    dataof := @dataof(fmt.value)
    bits := kindof.sizeof * 8

    var value: u64
    var signedValue: s64
    var signed: bool

    overflowS64 := false
    switch kindof
    {
        case s8:
            signedValue = cast(s64) dref cast(*s8) dataof
            value = cast(u64) Math.abs(signedValue)
        case s16:
            signedValue = cast(s64) dref cast(*s16) dataof
            value = cast(u64) Math.abs(signedValue)
        case s32:
            signedValue = cast(s64) dref cast(*s32) dataof
            value = cast(u64) Math.abs(signedValue)
        case s64, int:
            signedValue = dref cast(*s64) dataof
            // Cannot take the absolute value of Swag.S64.Min
            if signedValue == Swag.S64.Min
                overflowS64 = true
            else
                value = cast(u64) Math.abs(signedValue)
        case u8:
            value = cast(u64) dref cast(*u8) dataof
        case u16:
            value = cast(u64) dref cast(*u16) dataof
        case u32:
            value = cast(u64) dref cast(*u32) dataof
        case u64, uint:
            value = dref cast(*u64) dataof
    }

    const tbl = "0123456789ABCDEF"
    var tmp: [64] u8
    cpt := 63'u32

    // Special case for the lower digit of Swag.S64.Min
    // We have 9_223_372_036_854_775_807, and we want -9_223_372_036_854_775_808
    // So use '8' instead of '7'
    if overflowS64
    {
        c := tbl[8 % fmt.base]
        tmp[cpt] = c
        cpt, bits -= 1
        switch fmt.base
        {
        case 2:  value = 4611686018427387904
        case 10: value = 922337203685477580
        case 16: value = 576460752303423488
        }
    }

    loop bits
    {
        c := tbl[value % fmt.base]
        tmp[cpt] = c
        cpt -= 1
        value /= cast(u64) fmt.base
        if !value
            break
    }

    // Fill the remaining place with the padding character, so that the total result is 'fmt.width' long
    if fmt.padding
    {
        curWidth := 63 - cpt
        while curWidth < fmt.width and cpt > 1
        {
            tmp[cpt] = fmt.padding
            curWidth += 1
            cpt -= 1
        }
    }

    // Prepend the negative character
    if signedValue < 0
    {
        tmp[cpt] = cast(u8) g_CultureInfo.numberFormat.negativeSign
        cpt -= 1
    }
    else if fmt.forceSign
    {
        tmp[cpt] = cast(u8) g_CultureInfo.numberFormat.positiveSign
        cpt -= 1
    }

    buf.addBytes(cast([..] u8) @mkslice(&tmp[cpt + 1], cast(uint) 63 - cpt));
}

// Convert a float to an utf8 string, and put the result in a ConcatBuffer
func convertFloat(buf: *ConcatBuffer, value: any, strFormat: string)
{
    Debug.assert(@kindof(value).flags & TypeInfoFlags.Float != 0, "value to convert is not a float")

    var fmt: ConvertFormatFloat
    fmt.value = value
    fmt.setFormat(strFormat)
    convertFloat(buf, fmt)
}

// Convert an integer to an utf8 string, and put the result in a ConcatBuffer
func convertInt(buf: *ConcatBuffer, value: any, strFormat: string)
{
    Debug.assert(@kindof(value).flags & TypeInfoFlags.Integer != 0, "value to convert is not an integer")

    var fmt: ConvertFormatInt
    fmt.value = value
    fmt.setFormat(strFormat)
    convertInt(buf, fmt)
}

// Convert a 'bool' to a string, and put the result in a ConcatBuffer
func convertBool(buf: *ConcatBuffer, value: bool, strFormat: string)
{
    buf.addBytes(value ? "true" : "false")
}

// Convert a 'rune' to a string, and put the result in a ConcatBuffer
func convertRune(buf: *ConcatBuffer, value: rune, strFormat: string)
{
    var arr: [4] u8
    var count = Utf8.encodeRune(arr, value)
    buf.addBytes(@mkslice(&arr[0], cast(uint) count))
}
