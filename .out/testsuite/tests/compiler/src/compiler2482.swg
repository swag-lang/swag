using Swag

var G = 0
func panic(msg: string, loc: Swag.CompilerSourceLocation)
{
    G += 1
}

#[Safety("overflow", true)]
#[Optim("", false)]
//#[PrintBc, optim("bc", false)]
#test
{
    {
        cxt := @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            x := Swag.S16.Min
            x0 := cast(s8) x
        }

        {
            x := Swag.S16.Max
            x0 := cast(s8) x
        }

        {
            x := Swag.S32.Min
            x0 := cast(s8) x
            x1 := cast(s16) x
        }

        {
            x := Swag.S32.Max
            x0 := cast(s8) x
            x1 := cast(s16) x
        }

        {
            x := Swag.S64.Min
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
        }

        {
            x := Swag.S64.Max
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
        }
    }

    @assert(G == 12)
}