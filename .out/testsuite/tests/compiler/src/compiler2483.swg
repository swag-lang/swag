using Swag

var G = 0
func panic(msg: string, loc: Swag.CompilerSourceLocation)
{
    G += 1
}

#[Safety("overflow", true)]
#[optim("", false)]
//#[PrintBc]
#test
{
    {
        cxt := @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            x := Swag.U8.Max
            x0 := cast(s8) x
        }

        {
            x := Swag.U16.Max
            x0 := cast(s8) x
            x1 := cast(s16) x
        }

        {
            x := Swag.U32.Max
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
        }

        {
            x := Swag.U64.Max
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
            x3 := cast(s64) x
        }
    }

    @assert(G == 10)
}