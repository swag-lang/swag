namespace std.text
{
    using std.collections, std.diagnostics

    /// Format structure to convert an integer to a string
    ///
    /// # See also
    /// * [std.text.convertInt]
    public struct ConvertFormatInt
    {
        /// The unsigned  value to convert to string
        unsignedValue:  u64

        /// The signed  value to convert to string
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
        /// Convert an integer to a string, and put the result in a [ConcatBuffer]
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
                value /= 10
                if !value
                    break
            }

            if fmt.signed && value
            {
                tmp[cpt] = cast(u8) "-"'char
                cpt -= 1
            }

            buf.addBytes(cast([..] u8) {&tmp[cpt], 63 - cpt});
        }

        /// Convert an s32 integer to a string, and put the result in a [ConcatBuffer]
        func convertInt(buf: *ConcatBuffer, value: s32)
        {
            var fmt: ConvertFormatInt
            fmt.signedValue = acast value
            fmt.bits = 32
            convertInt(buf, fmt)
        }
    }
}
