namespace std.text
{
    using std.collections

    public struct ConvertFormatInt
    {
        unsignedValue:  u64
        #[swag.offset("unsignedValue")]
        signedValue:    s64

        bits:           u32 = 32
        base:           u32 = 10
        signed:         bool
    }

    func convertInt(buf: *ConcatBuffer, fmt: ConvertFormatInt)
    {
        const tbl = "0123456789ABCDEF"
        var value: u64 = fmt.signed ? acast -fmt.signedValue : fmt.unsignedValue

        diagnostics.debug.assert(fmt.bits > 0 && fmt.bits <= 64, "bad 'bits' value (should be > 0 and <= 64)")
        loop fmt.bits
        {
            //c := tbl[value % fmt.base]
        }
    }

    func convertInt(buf: *ConcatBuffer, value: s32)
    {
        var fmt: ConvertFormatInt
        fmt.signedValue = acast value
        fmt.bits = 32
        convertInt(buf, fmt)
    }
}
