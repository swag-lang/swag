var G = 0
func panic(msg: string, loc: Swag.SourceCodeLocation)
{
    G += 1
}

//#[Swag.PrintBc]
#[Swag.Safety(.All, .Overflow, true), Swag.Optimize(.All, false), Swag.Warning("Wrn0003", Swag.WarningLevel.Disable)]
#test
{
    {
        var cxt = @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            var x  = 0xFFFF'u16
            var x0 = cast(u8) x
        }

        {
            var x  = 0xFFFFFFFF'u32
            var x0 = cast(u8) x
            var x1 = cast(u16) x
        }

        {
            var x  = 0xFFFFFFFF_FFFFFFFF'u64
            var x0 = cast(u32) x
            var x1 = cast(u8) x
            var x2 = cast(u16) x
        }
    }

    @assert(G == 6)
}
