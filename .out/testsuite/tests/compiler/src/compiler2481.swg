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
            x := -1's8
            x0 := cast(u8) x
            x1 := cast(u16) x
            x2 := cast(u32) x
            x3 := cast(u64) x
        }

        {
            x := -1's16
            x0 := cast(u8) x
            x1 := cast(u16) x
            x2 := cast(u32) x
            x3 := cast(u64) x
        }

        {
            x := -1's32
            x0 := cast(u8) x
            x1 := cast(u16) x
            x2 := cast(u32) x
            x3 := cast(u64) x
        }

        {
            x := -1's64
            x0 := cast(u8) x
            x1 := cast(u16) x
            x2 := cast(u32) x
            x3 := cast(u64) x
        }

        {
            x := Swag.S16.Max
            x0 := cast(u8) x
        }

        {
            x := Swag.S32.Max
            x0 := cast(u8) x
            x1 := cast(u16) x
        }

        {
            x := Swag.S64.Max
            x0 := cast(u8) x
            x1 := cast(u16) x
            x2 := cast(u32) x
        }
    }

    @assert(G == 27)
}