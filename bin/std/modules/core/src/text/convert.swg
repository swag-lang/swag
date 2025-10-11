#global public
#global namespace StrConv
using Swag, Globalization

struct ConvertFormat
{
    formatInt:       ConvertFormatInt
    formatFloat:     ConvertFormatFloat
}

// Format structure to convert a float to a string
struct ConvertFormatFloat
{
    precision:     s8 = -1         // -1 is for 'most possible precision'
    fmt:           u8 = 'f''u8
    forceSign:     bool
}

impl ConvertFormatFloat
{
    // Set some format options with a given user string.
    // Format is `[+][fmt][precision]`
    //
    // + '+'
    //     To force the positive sign if the number is positive
    // + 'fmt'
    //     This is the output format:
    //     | e | -d.dddde±dd, a decimal exponent
    //     | E | -d.ddddE±dd, a decimal exponent
    //     | f | -ddd.dddd, no exponent
    //     | g | 'e' for large exponents, 'f' otherwise
    //     | G | 'E' for large exponents, 'f' otherwise
    // + 'precision'
    //     A number which is the precision of the fractional part
    mtd setFormat(strFormat: #null string)
    {
        let buffer  = @dataof(strFormat)
        let countof = @countof(strFormat)
        if buffer == null or countof == 0 do
            return

        var cpt = 0'u32

        // Force sign for positive numbers
        if buffer[cpt] == '+''u8
        {
            .forceSign = true
            cpt += 1
        }

        // Format
        if cpt < countof
        {
            switch buffer[cpt]
            {
            case 'e''u8, 'E''u8, 'f''u8, 'g''u8, 'G''u8:
                .fmt = buffer[cpt]
                cpt += 1
            }
        }

        // Precision
        if cpt < countof
        {
            let (userWidth, eatCount, ?) = assume parseU64(@mkstring(buffer + cpt, cast(u64) (countof - cpt)))
            Debug.assert(userWidth <= 255, "precision overflow")
            if eatCount do
                .precision = cast(s8) userWidth
        }
    }
}

// Format structure to convert an integer to a string
//
// If 'signed' is 'true', the value to convert must be stored in 'signedValue', otherwise it must
// be stored in 'unsignedValue'.
struct ConvertFormatInt
{
    base:          u32 = 10
    padding:       u8
    width:         u8
    forceSign:     bool
}

impl ConvertFormatInt
{
    // Set some format options with a given user string.
    // The format is `[+][fmt][padding][width]'
    //
    // + `+'
    //     To force the positive sign if the number is positive
    // + 'fmt'
    //     This is the output format:
    //     | B | Binary
    //     | D | Decimal
    //     | X | Hexadecimal
    // + 'padding'
    //     The padding character, in ascii. It's mandatory before the next field
    // + 'width'
    //     The width of the output, in number of characters
    mtd setFormat(strFormat: #null string)
    {
        let buffer  = @dataof(strFormat)
        let countof = @countof(strFormat)
        if buffer == null or countof == 0 do
            return

        var cpt = 0'u32

        // Force sign for positive numbers
        if buffer[cpt] == '+''u8
        {
            .forceSign = true
            cpt += 1
        }

        // Base
        switch buffer[cpt]
        {
        case 'B''u8:
            .base = 2
            cpt += 1

        case 'D''u8:
            .base = 10
            cpt += 1

        case 'X''u8:
            .base = 16
            cpt += 1
        }

        // Next comes the padding characters
        if cpt >= countof do
            return
        .padding = buffer[cpt]
        cpt += 1

        // Next comes the padding width
        if cpt >= countof do
            return
        let (userWidth, eatCount, ?) = assume parseU64(@mkstring(buffer + cpt, cast(u64) countof - cpt))
        Debug.assert(userWidth <= 255, "padding width overflow")
        if eatCount do
            .width = cast(u8) userWidth
    }
}

// Convert a value to an utf8 string, and append the result in a ConcatBuffer
func convertAny(buf: *ConcatBuffer, value: any, convFmt: ConvertFormat, strFormat: #null string)
{
    var valueData = @dataof(value)
    var type      = Reflection.makeConcreteAlias(@kindof(value))

    // A reference
    if Reflection.isPointerRef(type)
    {
        let ptrType = cast(const *TypeInfoPointer) type
        valueData = dref cast(**void) valueData
        type      = ptrType.pointedType
    }

    let concreteValue = @mkany(valueData, type)

    // This is a type
    if Reflection.isType(type)
    {
        let ptrType = dref cast(*typeinfo) valueData
        if ptrType do
            buf.addBytes(ptrType.fullname)
        else do
            buf.addBytes("null")
        return
    }

    // A pointer, whatever type
    if Reflection.isPointer(type)
    {
        if type.flags & TypeInfoFlags.CString
        {
            let ptr = dref cast(const [*] [*] u8) valueData
            let len = @strlen(ptr)
            buf.addBytes(@mkslice(ptr, len))
            return
        }

        var fmt: ConvertFormatInt
        fmt.base    = 16
        fmt.width   = 16
        fmt.padding = '0''u8
        fmt.setFormat(strFormat)
        convertInt(buf, dref cast(const *u64) valueData, fmt)
        return
    }

    // An enum value
    if Reflection.isEnum(type)
    {
        let enumName = Reflection.getEnumName(type, valueData)
        if !enumName.isEmpty() do
            buf.addBytes(@mkslice(enumName.buffer, cast(u64) enumName.length))
        else do
            buf.addBytes("?")
        return
    }

    // A struct
    if Reflection.isStruct(type)
    {
        convertStruct(buf, concreteValue, convFmt, strFormat)
        return
    }

    // A slice
    if Reflection.isSlice(type)
    {
        let ptrSlice = cast(const *TypeInfoSlice) type
        buf.addBytes("[")

        let sl       = cast(const [..] u8) valueData
        var bufSlice = @dataof(sl)
        for @countof(sl)
        {
            if @index do
                buf.addBytes(", ")
            let v = @mkany(bufSlice, ptrSlice.pointedType)
            convertAny(buf, v, convFmt, strFormat)
            bufSlice += ptrSlice.pointedType.sizeof
        }

        buf.addBytes("]")
        return
    }

    // A static array
    if Reflection.isStaticArray(type)
    {
        let ptrArr = cast(const *TypeInfoArray) type
        var bufArr = cast(const [*] u8) valueData

        buf.addBytes("[")
        for ptrArr.count
        {
            if @index do
                buf.addBytes(", ")
            let v = @mkany(bufArr, ptrArr.pointedType)
            convertAny(buf, v, convFmt, strFormat)
            bufArr += ptrArr.pointedType.sizeof
        }

        buf.addBytes("]")
        return
    }

    if Reflection.isInteger(type)
    {
        convertInt(buf, concreteValue, convFmt, strFormat)
        return
    }

    if Reflection.isFloat(type)
    {
        convertFloat(buf, concreteValue, convFmt, strFormat)
        return
    }

    // Any
    if Reflection.isAny(type)
    {
        let ptrAny = cast(const *any) valueData
        buf.addBytes("{type = ")
        let typeAny = @kindof(dref ptrAny)
        convertAny(buf, typeAny, convFmt, strFormat)
        buf.addBytes(", value = ")
        if @dataof(dref ptrAny) do
            convertAny(buf, dref ptrAny, convFmt, strFormat)
        else do
            buf.addBytes("null")
        buf.addBytes("}")
        return
    }

    switch type
    {
    case bool:
        convertBool(buf, cast(bool) concreteValue)

    case rune:
        convertRune(buf, cast(rune) concreteValue)

    case string:
        buf.addBytes(cast(string) concreteValue)

    default:
        buf.addBytes("?")
    }
}

// Convert a structure content to an utf8 string, and put the result in a ConcatBuffer
func convertStruct(buf: *ConcatBuffer, value: any, convFmt: ConvertFormat, strFormat: #null string)
{
    let kindof = cast(const *Swag.TypeInfoStruct) @kindof(value)
    Debug.assert(kindof.kind == TypeInfoKind.Struct, "this is not a structure")
    let ptr = cast(const [*] u8) @dataof(value)

    // Core.String
    if kindof == String
    {
        let ptrStr = cast(const *String) ptr
        buf.addBytes(@mkslice(ptrStr.buffer, ptrStr.length))
        return
    }

    // A structure with an implementation of IConvert
    let itf = @mkinterface(ptr, kindof, IConvert)
    if itf != null
    {
        itf.convert(buf, convFmt, strFormat)
        return
    }

    // Do it field by field
    buf.addBytes("{")
    foreach field in kindof.fields
    {
        if @index do
            buf.addBytes(", ")
        buf.addBytes(field.name)
        buf.addBytes(" = ")
        let val = @mkany(ptr + field.offset, field.pointedType)
        convertAny(buf, val, convFmt, strFormat)
    }
    buf.addBytes("}")
}

// Convert a float to an utf8 string, and put the result in a ConcatBuffer
#[Swag.Overload]
func convertFloat(buf: *ConcatBuffer, value: any, fmt: ConvertFormatFloat)
{
    let kindof = cast(const *Swag.TypeInfoNative) @kindof(value)
    Debug.assert(kindof.flags & TypeInfoFlags.Float != 0, "value to convert is not a float")

    let dataof = @dataof(value)

    var value64: f64
    switch kindof.nativeKind
    {
    case F32:
        value64 = cast(f64) dref cast(*f32) dataof
    case F64:
        value64 = dref cast(*f64) dataof
    }

    Dtoa.parse(buf, value64, fmt.fmt, cast(s64) fmt.precision, fmt.forceSign)
}

// Convert an integer to an utf8 string, and put the result in a ConcatBuffer
#[Swag.Overload]
func convertInt(buf: *ConcatBuffer, value: any, fmt: ConvertFormatInt)
{
    let kindof = cast(const *Swag.TypeInfoNative) @kindof(value)
    Debug.assert(kindof.flags & TypeInfoFlags.Integer != 0, "value to convert is not an integer")
    Debug.assert(0 < fmt.base and fmt.base <= 16, "invalid convert base")

    let dataof = @dataof(value)
    var bits   = kindof.sizeof * 8

    var value64:     u64
    var signedValue: s64

    var overflowS64 = false
    switch kindof.nativeKind
    {
    case S8:
        signedValue = cast(s64) dref cast(*s8) dataof
        value64     = cast(u64) Math.abs(signedValue)
    case S16:
        signedValue = cast(s64) dref cast(*s16) dataof
        value64     = cast(u64) Math.abs(signedValue)
    case S32:
        signedValue = cast(s64) dref cast(*s32) dataof
        value64     = cast(u64) Math.abs(signedValue)
    case S64:
        signedValue = dref cast(*s64) dataof
        if signedValue == Swag.S64.Min do
            overflowS64 = true
        else do
            value64 = cast(u64) Math.abs(signedValue)
    case U8:
        value64 = cast(u64) dref cast(*u8) dataof
    case U16:
        value64 = cast(u64) dref cast(*u16) dataof
    case U32:
        value64 = cast(u64) dref cast(*u32) dataof
    case U64:
        value64 = dref cast(*u64) dataof
    }

    const Tbl = "0123456789ABCDEF"
    var tmp:  [64] u8 = undefined
    var cpt   = 63'u32

    // Special case for the lower digit of Swag.S64.Min
    // We have 9_223_372_036_854_775_807, and we want -9_223_372_036_854_775_808
    // So use '8' instead of '7'
    if overflowS64
    {
        let c = Tbl[8 % fmt.base]
        tmp[cpt] = c
        cpt, bits -= 1
        switch fmt.base
        {
        case 2:
            value64 = 4611686018427387904
        case 10:
            value64 = 922337203685477580
        case 16:
            value64 = 576460752303423488
        }
    }

    for bits
    {
        let c = Tbl[value64 % fmt.base]
        tmp[cpt] = c
        cpt -= 1
        value64 /= cast(u64) fmt.base
        if !value64 do
            break
    }

    // Fill the remaining place with the padding character, so that the total result is 'fmt.width' long
    if fmt.padding
    {
        var curWidth = 63 - cpt
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
    elif fmt.forceSign
    {
        tmp[cpt] = cast(u8) g_CultureInfo.numberFormat.positiveSign
        cpt -= 1
    }

    buf.addBytes(cast([..] u8) @mkslice(&tmp[cpt + 1], cast(u64) 63 - cpt))
}

// Convert a float to an utf8 string, and put the result in a ConcatBuffer
#[Swag.Overload]
func convertFloat(buf: *ConcatBuffer, value: any, convFmt: ConvertFormat, strFormat: #null string)
{
    Debug.assert(@kindof(value).flags & TypeInfoFlags.Float != 0, "value to convert is not a float")

    var fmt = convFmt.formatFloat
    fmt.setFormat(strFormat)
    convertFloat(buf, value, fmt)
}

// Convert an integer to an utf8 string, and put the result in a ConcatBuffer
#[Swag.Overload]
func convertInt(buf: *ConcatBuffer, value: any, convFmt: ConvertFormat, strFormat: #null string)
{
    Debug.assert(@kindof(value).flags & TypeInfoFlags.Integer != 0, "value to convert is not an integer")

    var fmt = convFmt.formatInt
    fmt.setFormat(strFormat)
    convertInt(buf, value, fmt)
}

// Convert a 'bool' to a string, and put the result in a ConcatBuffer
func convertBool(buf: *ConcatBuffer, value: bool)
{
    buf.addBytes(value ? "true" : "false")
}

// Convert a 'rune' to a string, and put the result in a ConcatBuffer
func convertRune(buf: *ConcatBuffer, value: rune)
{
    var arr: [4] u8
    if !value do
        buf.addBytes("?")
    else
    {
        let count = Utf8.encodeRune(arr, value)
        buf.addBytes(@mkslice(&arr[0], cast(u64) count))
    }
}

// Templated version. Convert a value of type 'T'
#[Swag.Warning("Wrn0006", .Disable)]
func(T) convert(buf: *ConcatBuffer, value: T, convFmt: ConvertFormat, strFormat: #null string)
{
    #if Reflection.isInteger(T)
    {
        var fmt: ConvertFormatInt
        fmt.setFormat(strFormat)
        convertInt(buf, value, fmt)
    }
    #elif Reflection.isFloat(T)
    {
        var fmt: ConvertFormatFloat
        fmt.setFormat(strFormat)
        convertFloat(buf, value, fmt)
    }
    #elif Reflection.isBool(T)
    {
        convertBool(buf, value)
    }
    #elif Reflection.isRune(T)
    {
        convertRune(buf, value)
    }
    #elif Reflection.isString(T)
    {
        buf.addBytes(value)
    }
    #else
    {
        convertAny(buf, value, convFmt, strFormat)
    }
}
