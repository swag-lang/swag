using Swag

var G = 0
func panic(msg: string, loc: Swag.SourceCodeLocation)
{
    G += 1
}

#[Safety("overflow", true)]
#[Optim("", false)]
//#[PrintBc, optim("bc", false)]
#test
{
    {
        let cxt = @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            let x = Swag.S16.Min
            let x0 = cast(s8) x
        }

        {
            let x = Swag.S16.Max
            let x0 = cast(s8) x
        }

        {
            let x = Swag.S32.Min
            let x0 = cast(s8) x
            let x1 = cast(s16) x
        }

        {
            let x = Swag.S32.Max
            let x0 = cast(s8) x
            let x1 = cast(s16) x
        }

        {
            let x = Swag.S64.Min
            let x0 = cast(s8) x
            let x1 = cast(s16) x
            let x2 = cast(s32) x
        }

        {
            let x = Swag.S64.Max
            let x0 = cast(s8) x
            let x1 = cast(s16) x
            let x2 = cast(s32) x
        }
    }

    @assert(G == 12)
}