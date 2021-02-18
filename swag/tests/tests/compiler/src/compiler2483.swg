using swag

var G = 0
func panic(msg: string, loc: swag.CompilerSourceLocation)
{
    G += 1
}

#[safety("of", true)]
//#[printbc]
#test
{
    {
        cxt := @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            x := swag.MaxU8
            x0 := cast(s8) x
        }

        {
            x := swag.MaxU16
            x0 := cast(s8) x
            x1 := cast(s16) x
        }

        {
            x := swag.MaxU32
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
        }

        {
            x := swag.MaxU64
            x0 := cast(s8) x
            x1 := cast(s16) x
            x2 := cast(s32) x
            x3 := cast(s64) x
        }
    }

    @assert(G == 10)
}