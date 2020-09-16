namespace std.text
{
    using swag
    using std.collections, std.diagnostics, std.globalization

    public struct ConvertFormat
    {
        value: any
    }

    // Format structure to convert a float to a string
    public struct ConvertFormatFloat
    {
        using val:  ConvertFormat
        precision:  u32 = 5
    }

    // Format structure to convert an integer to a string
    // If 'signed' is 'true', the value to convert must be stored in 'signedValue', otherwise it must
    // be stored in 'unsignedValue'.
    public struct ConvertFormatInt
    {
        using val:  ConvertFormat
        base:       u32 = 10
        padding:    u8
        width:      u8
    }

    impl ConvertFormatFloat
    {
        // Set some format options with a given user string
        public func setFormat(using self, strFormat: string)
        {
            if strFormat == null
                return
        }
    }

    impl ConvertFormatInt
    {
        // Set some format options with a given user string
        // Format is :
        // [B|D|X] [padding] [width]
        // => binary, decimal, hexadecimal
        // => padding character, in ascii, mandatory before the next field
        // => width of the output, in characters
        public func setFormat(using self, strFormat: string)
        {
            buffer := @dataof(strFormat)
            countof := @countof(strFormat)
            if buffer == null || countof == 0
                return

            cpt := 0'u32
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
            (userWidth, eatCount) := parseU64(@mkslice(buffer + cpt, countof - cpt))
            debug.assert(userWidth <= 255, "padding width overflow")
            width = cast(u8) userWidth
        }
    }

    // Interface to implement a custom conversion function
    public interface IConvert
    {
        convert: func(self, *ConcatBuffer, const *ConvertFormat, string)
    }

    public
    {
        // Convert a value to an utf8 string, and append the result in a ConcatBuffer
        func convertAny(buf: *ConcatBuffer, value: any, strFormat: string)
        {
            type := cast(const *TypeInfo) @kindof(value)

            // A pointer, whatever type
            if type.kind == TypeInfoKind.Pointer
            {
                buf.addBytes("?")
                return
            }

            if type.kind == TypeInfoKind.Struct
            {
                // A special format structure
                if type == ConvertFormatInt
                    convertInt(buf, cast(ConvertFormatInt) value)
                else if type == ConvertFormatFloat
                    convertFloat(buf, cast(ConvertFormatFloat) value)
                else
                {
                    // A structure with an implementation of IConvert
                    itf := @mkinterface(@dataof(value), @kindof(value), IConvert)
                    if itf != null
                        itf.convert(buf, null, strFormat)
                    else
                        convertStruct(buf, value, strFormat)
                }

                return
            }

            if type.flags & TypeInfoFlags.Integer
                convertInt(buf, value, strFormat)
            else if type.flags & TypeInfoFlags.Float
                convertFloat(buf, value, strFormat)
            else switch type
            {
                case bool:
                    convertBool(buf, cast(bool) value, strFormat)
                case char:
                    convertChar(buf, cast(char) value, strFormat)
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
            debug.assert(kindof.kind == TypeInfoKind.Struct, "this is not a structure")

            buf.addBytes("{")

            ptr := @dataof(value)
            type := cast(const *TypeInfoStruct) kindof
            visit field: type.fields
            {
                if(@index != 0)
                    buf.addBytes(",")
                buf.addBytes(field.name)
                buf.addBytes("=")
                val := @mkany(ptr + field.offset, field.pointedType)
                convertAny(buf, val, strFormat)
            }

            buf.addBytes("}")
        }

        // Convert an integer to an utf8 string, and put the result in a ConcatBuffer
        func convertFloat(buf: *ConcatBuffer, fmt: ConvertFormatFloat)
        {
            kindof := @kindof(fmt.value)
            debug.assert(kindof.flags & TypeInfoFlags.Float != 0, "value to convert is not a float")

            dataof := @dataof(fmt.value)
            bits := kindof.sizeof * 8

            var value: f64
            switch kindof
            {
                case f32: value = cast(f64) deref cast(*f32) dataof
                case f64: value = deref cast(*f64) dataof
            }

            neg := value < 0

            if(neg)
            {
                value = -value
                buf.addByte(cast(u8) g_CultureInfo.numberFormat.negativeSign)
            }

            // Round with precision
            var roundValue: f64 = 0.5
            precision := math.min(fmt.precision, 100)
            if precision
                roundValue = 0.5'f64 / math.pow(cast(f64) precision, 10'f64)
            value += roundValue

            // Convert integer part
            intPart := cast(u64) value
            var fmtInt: ConvertFormatInt
            fmtInt.value = intPart
            convertInt(buf, fmtInt)

            // Decimal part
            value -= cast(f64) intPart
            if precision
                buf.addByte(cast(u8) g_CultureInfo.numberFormat.decimalSeparator);

            loop precision
            {
                value *= 10
                c := cast(u32) value
                buf.addByte(cast(u8) (c + "0"'u8))
                value -= cast(f64) c
            }
        }

        // Convert an integer to an utf8 string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, fmt: ConvertFormatInt)
        {
            kindof := @kindof(fmt.value)
            debug.assert(kindof.flags & TypeInfoFlags.Integer != 0, "value to convert is not an integer")
            debug.assert(0 < fmt.base <= 16, "invalid convert base")

            dataof := @dataof(fmt.value)
            bits := kindof.sizeof * 8

            var value: u64
            var signedValue: s64
            var signed: bool

            switch kindof
            {
                case s8:    signedValue = cast(s64) deref cast(*s8) dataof; value = cast(u64) math.abs(signedValue)
                case s16:   signedValue = cast(s64) deref cast(*s16) dataof; value = cast(u64) math.abs(signedValue)
                case s32:   signedValue = cast(s64) deref cast(*s32) dataof; value = cast(u64) math.abs(signedValue)
                case s64:   signedValue = deref cast(*s64) dataof; value = cast(u64) math.abs(signedValue)
                case u8:    value = cast(u64) deref cast(*u8) dataof;
                case u16:   value = cast(u64) deref cast(*u16) dataof
                case u32:   value = cast(u64) deref cast(*u32) dataof
                case u64:   value = deref cast(*u64) dataof
            }

            const tbl = "0123456789ABCDEF"
            var tmp: [64] u8
            cpt := 63'u32
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
                while curWidth < fmt.width && cpt > 1
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

            buf.addBytes(cast([..] u8) @mkslice(&tmp[cpt + 1], 63 - cpt));
        }

        // Convert a float to an utf8 string, and put the result in a ConcatBuffer
        func convertFloat(buf: *ConcatBuffer, value: any, strFormat: string)
        {
            debug.assert(@kindof(value).flags & TypeInfoFlags.Float != 0, "value to convert is not a float")

            var fmt: ConvertFormatFloat
            fmt.value = value
            fmt.setFormat(strFormat)
            convertFloat(buf, fmt)
        }

        // Convert an integer to an utf8 string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, value: any, strFormat: string)
        {
            debug.assert(@kindof(value).flags & TypeInfoFlags.Integer != 0, "value to convert is not an integer")

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

        // Convert a 'char' to a string, and put the result in a ConcatBuffer
        func convertChar(buf: *ConcatBuffer, value: char, strFormat: string)
        {
            if value <= 0xFF
                buf.addByte(cast(u8) value)
            else if(value  <= 0xFFFF)
                buf.addBytes(@mkslice(cast(const *u8) &value, 2))
            else if(value  <= 0xFFFFFF)
                buf.addBytes(@mkslice(cast(const *u8) &value, 3))
            else
                buf.addBytes(@mkslice(cast(const *u8) &value, 4))
        }
    }
}
