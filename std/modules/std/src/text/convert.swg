namespace std.text
{
    using std.collections, std.diagnostics

    public struct ConvertFormatInt
    {
        unsignedValue:  u64
        #[swag.offset("unsignedValue")]
        signedValue:    s64

        bits:           u32 = 32
        base:           u32 = 10
        signed:         bool
    }

    public 
    {
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

        func convertInt(buf: *ConcatBuffer, value: s32)
        {
            var fmt: ConvertFormatInt
            fmt.signedValue = acast value
            fmt.bits = 32
            convertInt(buf, fmt)
        }
    }
}
