using Swag

var G = 0
func panic(msg: string, loc: Swag.SourceCodeLocation)
{
    G += 1
}

#[Safety(.All, .Overflow, true), Optimize(.All, false), Swag.Warning("Wrn0003", Swag.WarningLevel.Disable)]
#test
{
    {
        var cxt = @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            var x  = -1's8
            var x0 = cast(u8) x
            var x1 = cast(u16) x
            var x2 = cast(u32) x
            var x3 = cast(u64) x
        }

        {
            var x  = -1's16
            var x0 = cast(u8) x
            var x1 = cast(u16) x
            var x2 = cast(u32) x
            var x3 = cast(u64) x
        }

        {
            var x  = -1's32
            var x0 = cast(u8) x
            var x1 = cast(u16) x
            var x2 = cast(u32) x
            var x3 = cast(u64) x
        }

        {
            var x  = -1's64
            var x0 = cast(u8) x
            var x1 = cast(u16) x
            var x2 = cast(u32) x
            var x3 = cast(u64) x
        }

        {
            var x  = Swag.S16.Max
            var x0 = cast(u8) x
        }

        {
            var x  = Swag.S32.Max
            var x0 = cast(u8) x
            var x1 = cast(u16) x
        }

        {
            var x  = Swag.S64.Max
            var x0 = cast(u8) x
            var x1 = cast(u16) x
            var x2 = cast(u32) x
        }
    }

    @assert(G == 28)
}
