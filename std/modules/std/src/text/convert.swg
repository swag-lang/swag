namespace std.text
{
    using std.collections, std.diagnostics

    /// Format structure to convert an integer to a string
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

        /// Number of valid bits in the value
        bits:           u32 = 32
        /// Base of the conversion (2 for binary, 10 for decimal, 16 for hexadecimal)
        base:           u32 = 10
        /// Is the value signed or unsigned
        signed:         bool
    }

    public 
    {
        /// Convert a value to an utf8 string, and put the result in a [ConcatBuffer]
        func convertAny(buf: *ConcatBuffer, value: any)
        {
            type := @kindof(value)
            ptr := cast(*s32) @dataof(value)
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
            }
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
                tmp[cpt] = cast(u8) "-"'char
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
