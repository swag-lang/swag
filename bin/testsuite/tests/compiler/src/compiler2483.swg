using Swag

var G = 0
func panic(msg: string, loc: Swag.SourceCodeLocation)
{
    G += 1
}

#[Safety("overflow", true)]
#[Optim("", false)]
//#[PrintBc]
#test
{
    {
        let cxt = @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            var x = Swag.U8.Max
            var x0 = cast(s8) x
        }

        {
            var x = Swag.U16.Max
            var x0 = cast(s8) x
            var x1 = cast(s16) x
        }

        {
            var x = Swag.U32.Max
            var x0 = cast(s8) x
            var x1 = cast(s16) x
            var x2 = cast(s32) x
        }

        {
            var x = Swag.U64.Max
            var x0 = cast(s8) x
            var x1 = cast(s16) x
            var x2 = cast(s32) x
            var x3 = cast(s64) x
        }
    }

    @assert(G == 10)
}