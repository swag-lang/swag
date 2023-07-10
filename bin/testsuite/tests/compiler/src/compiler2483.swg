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
            let x = Swag.U8.Max
            let x0 = cast(s8) x
        }

        {
            let x = Swag.U16.Max
            let x0 = cast(s8) x
            let x1 = cast(s16) x
        }

        {
            let x = Swag.U32.Max
            let x0 = cast(s8) x
            let x1 = cast(s16) x
            let x2 = cast(s32) x
        }

        {
            let x = Swag.U64.Max
            let x0 = cast(s8) x
            let x1 = cast(s16) x
            let x2 = cast(s32) x
            let x3 = cast(s64) x
        }
    }

    @assert(G == 10)
}