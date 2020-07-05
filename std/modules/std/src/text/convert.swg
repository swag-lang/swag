namespace std.text
{
    using std.collections, std.diagnostics, std.globalization

    /// Format structure to convert a float to a string
    ///
    /// # See also
    /// * [std.text.convertFloat]
    public struct ConvertFormatFloat
    {
        /// The  value to convert to string
        value:      f64
        /// Number of valid bits in the value (32 or 64)
        bits:       u32 = 32
        /// Number of digits in the decimal part
        precision:  u32 = 5
    }

    /// Format structure to convert an integer to a string
    ///
    /// # Remarks
    /// If `signed` is `true`, the value to convert must be stored in `signedValue`, otherwise it must
    /// be stored in `unsignedValue`.
    ///
    /// # See also
    /// * [std.text.convertInt]
    public struct ConvertFormatInt
    {
        /// The unsigned value to convert to string (`signed` must be `false`)
        unsignedValue:  u64

        /// The signed  value to convert to string (`signed` must be `true`)
        #[swag.offset("unsignedValue")]
        signedValue:    s64

        /// Number of valid bits in the value (usually 8, 16, 32 or 64)
        bits:           u32 = 32
        /// Base of the conversion (2 for binary, 10 for decimal, 16 for hexadecimal)
        base:           u32 = 10
        /// `true` if the value to convert is signed, `false` if it is unsigned
        signed:         bool
    }

    public 
    {
        /// Convert a value to an utf8 string, and put the result in a [ConcatBuffer]
        func convertAny(buf: *ConcatBuffer, value: any)
        {
            pp := cast(*ConvertFormatFloat) @dataof(value)
            @print(pp.value)

/*
            type := @kindof(value)
            switch type
            {
                case @typeof(s8):
                    convertInt(buf, cast(s8) value)                
                case @typeof(s16):
                    convertInt(buf, cast(s16) value)                     
                case @typeof(s32):
                    convertInt(buf, cast(s32) value)
                case @typeof(s64):
                    convertInt(buf, cast(s64) value) 
                case @typeof(u8):
                    convertInt(buf, cast(u8) value)                                        
                case @typeof(u16):
                    convertInt(buf, cast(u16) value)                     
                case @typeof(u32):
                    convertInt(buf, cast(u32) value)                    
                case @typeof(u64):
                    convertInt(buf, cast(u64) value)
                case @typeof(f32):
                    convertFloat(buf, cast(f32) value)
                case @typeof(f64):
                    convertFloat(buf, cast(f64) value)
                case @typeof(string):
                    buf.addBytes(cast(string) value)   
                case @typeof(ConvertFormatFloat):  
                    pp := cast(ConvertFormatFloat) value
                    @print(pp.value)
                    @print(" ")
                    @print(cast(s64) pp.bits)
                    //convertFloat(buf, cast(ConvertFormatFloat) value)
            }*/
        }

        /// Convert an integer to an utf8 string, and put the result in a [std.collections.ConcatBuffer]
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

        /// Convert a `f64` to a string, and put the result in a [std.collections.ConcatBuffer]
        func convertFloat(buf: *ConcatBuffer, value: f64)
        {
            var fmt: ConvertFormatFloat
            fmt.value = value
            fmt.bits = 64
            convertFloat(buf, fmt)
        }   

        /// Convert a `f32` to a string, and put the result in a [std.collections.ConcatBuffer]
        func convertFloat(buf: *ConcatBuffer, value: f32)
        {
            var fmt: ConvertFormatFloat
            fmt.value = cast(f64) value
            fmt.bits = 32
            convertFloat(buf, fmt)
        }          

        /// Convert an integer to an utf8 string, and put the result in a [std.collections.ConcatBuffer]
        func convertInt(buf: *ConcatBuffer, fmt: ConvertFormatInt)
        {
            const tbl = "0123456789ABCDEF"
            var value: u64 = fmt.signed ? cast(u64) math.abs(fmt.signedValue) : fmt.unsignedValue

            debug.assert(fmt.bits > 0 && fmt.bits <= 64, "bad 'bits' value (should be > 0 and <= 64)")

            var tmp: [64] u8
            cpt := 63
            loop fmt.bits
            {
                c := tbl[value % fmt.base]
                tmp[cpt] = c
                cpt -= 1
                value /= cast(u64) fmt.base
                if !value
                    break
            }

            if fmt.signed && fmt.signedValue < 0
            {
                tmp[cpt] = cast(u8) g_CultureInfo.numberFormat.negativeSign
                cpt -= 1
            }

            buf.addBytes(cast([..] u8) {&tmp[cpt + 1], 63 - cpt});
        }

        /// Convert a `s8` integer to a string, and put the result in a [std.collections.ConcatBuffer]
        func convertInt(buf: *ConcatBuffer, value: s8)
        {
            var fmt: ConvertFormatInt
            fmt.signedValue = acast value
            fmt.bits = 8
            fmt.signed = true
            convertInt(buf, fmt)
        }

        /// Convert a `s16` integer to a string, and put the result in a [std.collections.ConcatBuffer]
        func convertInt(buf: *ConcatBuffer, value: s16)
        {
            var fmt: ConvertFormatInt
            fmt.signedValue = acast value
            fmt.bits = 16
            fmt.signed = true
            convertInt(buf, fmt)
        }        

        /// Convert a `s32` integer to a string, and put the result in a [std.collections.ConcatBuffer]
        func convertInt(buf: *ConcatBuffer, value: s32)
        {
            var fmt: ConvertFormatInt
            fmt.signedValue = acast value
            fmt.bits = 32
            fmt.signed = true
            convertInt(buf, fmt)
        }

        /// Convert a `s64` integer to a string, and put the result in a [std.collections.ConcatBuffer]
        func convertInt(buf: *ConcatBuffer, value: s64)
        {
            var fmt: ConvertFormatInt
            fmt.signedValue = acast value
            fmt.bits = 64
            fmt.signed = true
            convertInt(buf, fmt)
        }        

        /// Convert a `u8` integer to a string, and put the result in a [std.collections.ConcatBuffer]
        func convertInt(buf: *ConcatBuffer, value: u8)
        {
            var fmt: ConvertFormatInt
            fmt.unsignedValue = acast value
            fmt.signed = false
            fmt.bits = 8
            convertInt(buf, fmt)
        }   

        /// Convert a `u16` integer to a string, and put the result in a [std.collections.ConcatBuffer]
        func convertInt(buf: *ConcatBuffer, value: u16)
        {
            var fmt: ConvertFormatInt
            fmt.unsignedValue = acast value
            fmt.signed = false
            fmt.bits = 16
            convertInt(buf, fmt)
        }           

        /// Convert a `u32` integer to a string, and put the result in a [std.collections.ConcatBuffer]
        func convertInt(buf: *ConcatBuffer, value: u32)
        {
            var fmt: ConvertFormatInt
            fmt.unsignedValue = acast value
            fmt.signed = false
            fmt.bits = 32
            convertInt(buf, fmt)
        }   

        /// Convert a `u64` integer to a string, and put the result in a [std.collections.ConcatBuffer]
        func convertInt(buf: *ConcatBuffer, value: u64)
        {
            var fmt: ConvertFormatInt
            fmt.unsignedValue = acast value
            fmt.signed = false
            fmt.bits = 64
            convertInt(buf, fmt)
        }          
    }
}
