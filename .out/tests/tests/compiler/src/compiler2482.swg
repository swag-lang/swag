using swag

var G = 0
func panic(msg: string, loc: swag.CompilerSourceLocation)
{
    G += 1
}

#[safety("overflow", true)]
#[optim("", false)]
//#[printbc, optim("bc", false)]
#test
{
    {
        cxt := @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            x := swag.S16.Min
            x0 := cast(s8) x
        }

        {
            x := swag.S16.Max
            x0 := cast(s8) x
        }

        {
            x := swag.S32.Min
            x0 := cast(s8) x
            x1 := cast(s16) x
        }

        {
            x := swag.S32.Max
            x0 := cast(s8) x
            x1 := cast(s16) x
        }

        {
            x := swag.S64.Min
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
        }

        {
            x := swag.S64.Max
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
        }
    }

    @assert(G == 12)
}