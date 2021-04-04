using swag

var G = 0
func panic(msg: string, loc: swag.CompilerSourceLocation)
{
    G += 1
}

#[safety("of", true)]
#[optim("", false)]
//#[printbc, optim("bc", false)]
#test
{
    {
        cxt := @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            x := swag.MinS16
            x0 := cast(s8) x
        }

        {
            x := swag.MaxS16
            x0 := cast(s8) x
        }

        {
            x := swag.MinS32
            x0 := cast(s8) x
            x1 := cast(s16) x
        }

        {
            x := swag.MaxS32
            x0 := cast(s8) x
            x1 := cast(s16) x
        }

        {
            x := swag.MinS64
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
        }

        {
            x := swag.MaxS64
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
        }
    }

    @assert(G == 12)
}