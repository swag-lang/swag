using Swag

var G = 0
func panic(msg: string, loc: Swag.SourceCodeLocation)
{
    G += 1
}

#[Safety("overflow", true), Optim("", false), Swag.Warn("Wrn0003", Swag.WarnLevel.Disable)]
#test
{
    {
        var cxt = @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            var x  = Swag.S16.Min
            var x0 = cast(s8) x
        }

        {
            var x  = Swag.S16.Max
            var x0 = cast(s8) x
        }

        {
            var x  = Swag.S32.Min
            var x0 = cast(s8) x
            var x1 = cast(s16) x
        }

        {
            var x  = Swag.S32.Max
            var x0 = cast(s8) x
            var x1 = cast(s16) x
        }

        {
            var x  = Swag.S64.Min
            var x0 = cast(s8) x
            var x1 = cast(s16) x
            var x2 = cast(s32) x
        }

        {
            var x  = Swag.S64.Max
            var x0 = cast(s8) x
            var x1 = cast(s16) x
            var x2 = cast(s32) x
        }
    }

    @assert(G == 12)
}
