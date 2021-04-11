using swag

var G = 0
func panic(msg: string, loc: swag.CompilerSourceLocation)
{
    G += 1
}

#[safety("of", true)]
#[optim("", false)]
//#[printbc]
#test
{
    {
        cxt := @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            x := swag.U8.Max
            x0 := cast(s8) x
        }

        {
            x := swag.U16.Max
            x0 := cast(s8) x
            x1 := cast(s16) x
        }

        {
            x := swag.U32.Max
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
        }

        {
            x := swag.U64.Max
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
            x3 := cast(s64) x
        }
    }

    @assert(G == 10)
}