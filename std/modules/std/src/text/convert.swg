namespace std.text
{
    using swag
    using std.collections, std.diagnostics, std.globalization

    // Format structure to convert a float to a string
    public struct ConvertFormatFloat
    {
        value:      f64
        bits:       u32 = 32
        precision:  u32 = 5
    }

    // Format structure to convert an integer to a string
    // If 'signed' is 'true', the value to convert must be stored in 'signedValue', otherwise it must
    // be stored in 'unsignedValue'.
    public struct ConvertFormatInt
    {
        unsignedValue:  u64
        #[offset("unsignedValue")]
        signedValue:    s64

        bits:           u32 = 32
        base:           u32 = 10
        signed:         bool
        padding:        u8
        width:          u8
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
            if strFormat == null || @countof(strFormat) == 0
                return

            cpt := 0'u32
            switch strFormat[cpt]
            {
                case "B"'u8: base = 2; cpt += 1
                case "D"'u8: base = 10; cpt += 1
                case "X"'u8: base = 16; cpt += 1
            }

            // Next comes the padding characters
            if cpt >= @countof(strFormat)
                return
            padding = strFormat[cpt]
            cpt += 1

            // Next comes the width
            if cpt >= @countof(strFormat)
                return
            width = 0
            while latin1.isDigit(strFormat[cpt]) && cpt < @countof(strFormat)
            {
                width *= 10
                width += cast(u8) (strFormat[cpt] - "0"'u8)
                cpt += 1
            }
        }
    }

    // Interface to implement a custom conversion function
    public interface IConvert
    {
        convert: func(self, *ConcatBuffer, string)
    }

    public
    {
        // Convert a value to an utf8 string, and append the result in a ConcatBuffer
        func convertAny(buf: *ConcatBuffer, value: any, strFormat: string = null)
        {
            type := @kindof(value)

/*            if type.kind == TypeInfoKind.Reference
            {
                typeRef := cast(const *TypeInfoReference) type
            }*/

            // A pointer, whatever type
            if type.kind == TypeInfoKind.Pointer
            {
                buf.addBytes("?")
                return
            }

            if type.kind == TypeInfoKind.Struct
            {
                // A structure with an implementation of IConvert
                itf := @mkinterface(@dataof(value), @kindof(value), IConvert)
                if itf != null
                {
                    itf.convert(buf, strFormat)
                    return
                }

                // A special format structure
                if type == ConvertFormatInt
                    convertInt(buf, cast(ConvertFormatInt) value)
                else if type ==  ConvertFormatFloat
                    convertFloat(buf, cast(ConvertFormatFloat) value)
                else
                    convertStruct(buf, value, strFormat)
                return
            }

            switch type
            {
                case bool:
                    convertBool(buf, cast(bool) value, strFormat)
                case char:
                    convertChar(buf, cast(char) value, strFormat)
                case s8:
                    convertInt(buf, cast(s8) value, strFormat)
                case s16:
                    convertInt(buf, cast(s16) value, strFormat)
                case s32:
                    convertInt(buf, cast(s32) value, strFormat)
                case s64:
                    convertInt(buf, cast(s64) value, strFormat)
                case u8:
                    convertInt(buf, cast(u8) value, strFormat)
                case u16:
                    convertInt(buf, cast(u16) value, strFormat)
                case u32:
                    convertInt(buf, cast(u32) value, strFormat)
                case u64:
                    convertInt(buf, cast(u64) value, strFormat)
                case f32:
                    convertFloat(buf, cast(f32) value, strFormat)
                case f64:
                    convertFloat(buf, cast(f64) value, strFormat)
                case string:
                    buf.addBytes(cast(string) value)
                default:
                    buf.addBytes("?")
            }
        }

        // Convert a structure content to an utf8 string, and put the result in a ConcatBuffer
        func convertStruct(buf: *ConcatBuffer, value: any, strFormat: string = null)
        {
            typeOf := @kindof(value)
            debug.assert(typeOf.kind == TypeInfoKind.Struct, "this is not a structure")

            buf.addBytes("{")

            ptr := @dataof(value)
            type := cast(const *TypeInfoStruct) typeOf
            visit field: type.fields
            {
                if(@index != 0)
                    buf.addBytes(",")
                buf.addBytes(field.name)
                buf.addBytes("=")
                val := @mkany(ptr + field.offset, field.pointedType)
                convertAny(buf, val)
            }

            buf.addBytes("}")
        }

        // Convert an integer to an utf8 string, and put the result in a ConcatBuffer
        func convertFloat(buf: *ConcatBuffer, fmt: ConvertFormatFloat)
        {
            neg := fmt.value < 0
            value := fmt.value

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
            fmtInt.signed = false
            fmtInt.bits = fmt.bits
            fmtInt.unsignedValue = intPart
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

        // Convert a 'f64' to a string, and put the result in a ConcatBuffer
        func convertFloat(buf: *ConcatBuffer, value: f64, strFormat: string = null)
        {
            var fmt: ConvertFormatFloat
            fmt.value = value
            fmt.bits = 64
            fmt.setFormat(strFormat)
            convertFloat(buf, fmt)
        }

        // Convert a 'f32' to a string, and put the result in a ConcatBuffer
        func convertFloat(buf: *ConcatBuffer, value: f32, strFormat: string = null)
        {
            var fmt: ConvertFormatFloat
            fmt.value = cast(f64) value
            fmt.bits = 32
            fmt.setFormat(strFormat)
            convertFloat(buf, fmt)
        }

        // Convert an integer to an utf8 string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, fmt: ConvertFormatInt)
        {
            const tbl = "0123456789ABCDEF"
            var value: u64 = fmt.signed ? cast(u64) math.abs(fmt.signedValue) : fmt.unsignedValue

            debug.assert(0 < fmt.bits <= 64, "bad 'bits' value (should be > 0 and <= 64)")

            var tmp: [64] u8
            cpt := 63'u32
            loop fmt.bits
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
            if fmt.signed && fmt.signedValue < 0
            {
                tmp[cpt] = cast(u8) g_CultureInfo.numberFormat.negativeSign
                cpt -= 1
            }

            buf.addBytes(cast([..] u8) @mkslice(&tmp[cpt + 1], 63 - cpt));
        }

        // Convert a 's8' integer to a string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, value: s8, strFormat: string = null)
        {
            var fmt: ConvertFormatInt
            fmt.signedValue = acast value
            fmt.bits = 8
            fmt.signed = true
            fmt.setFormat(strFormat)
            convertInt(buf, fmt)
        }

        // Convert a 's16' integer to a string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, value: s16, strFormat: string = null)
        {
            var fmt: ConvertFormatInt
            fmt.signedValue = acast value
            fmt.bits = 16
            fmt.signed = true
            fmt.setFormat(strFormat)
            convertInt(buf, fmt)
        }

        // Convert a 's32' integer to a string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, value: s32, strFormat: string = null)
        {
            var fmt: ConvertFormatInt
            fmt.signedValue = acast value
            fmt.bits = 32
            fmt.signed = true
            fmt.setFormat(strFormat)
            convertInt(buf, fmt)
        }

        // Convert a 's64' integer to a string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, value: s64, strFormat: string = null)
        {
            var fmt: ConvertFormatInt
            fmt.signedValue = acast value
            fmt.bits = 64
            fmt.signed = true
            fmt.setFormat(strFormat)
            convertInt(buf, fmt)
        }

        // Convert a 'u8' integer to a string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, value: u8, strFormat: string = null)
        {
            var fmt: ConvertFormatInt
            fmt.unsignedValue = acast value
            fmt.signed = false
            fmt.bits = 8
            fmt.setFormat(strFormat)
            convertInt(buf, fmt)
        }

        // Convert a 'u16' integer to a string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, value: u16, strFormat: string = null)
        {
            var fmt: ConvertFormatInt
            fmt.unsignedValue = acast value
            fmt.signed = false
            fmt.bits = 16
            fmt.setFormat(strFormat)
            convertInt(buf, fmt)
        }

        // Convert a 'u32' integer to a string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, value: u32, strFormat: string = null)
        {
            var fmt: ConvertFormatInt
            fmt.unsignedValue = acast value
            fmt.signed = false
            fmt.bits = 32
            fmt.setFormat(strFormat)
            convertInt(buf, fmt)
        }

        // Convert a 'u64' integer to a string, and put the result in a ConcatBuffer
        func convertInt(buf: *ConcatBuffer, value: u64, strFormat: string = null)
        {
            var fmt: ConvertFormatInt
            fmt.unsignedValue = acast value
            fmt.signed = false
            fmt.bits = 64
            fmt.setFormat(strFormat)
            convertInt(buf, fmt)
        }

        // Convert a 'bool' to a string, and put the result in a ConcatBuffer
        func convertBool(buf: *ConcatBuffer, value: bool, strFormat: string = null)
        {
            buf.addBytes(value ? "true" : "false")
        }

        // Convert a 'char' to a string, and put the result in a ConcatBuffer
        func convertChar(buf: *ConcatBuffer, value: char, strFormat: string = null)
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
