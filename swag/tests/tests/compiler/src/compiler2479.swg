var G = 0
func panic(msg: string, loc: swag.CompilerSourceLocation)
{
    G += 1;
}

//#[swag.printbc]
#[swag.safety("of", true)]
#test
{
    {
        cxt := @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            x := 0xFFFF'u16
            x0 := cast(u8) x
        }

        {
            x := 0xFFFFFFFF'u32
            x0 := cast(u8) x
            x1 := cast(u16) x
        }

        {
            x := 0xFFFFFFFF_FFFFFFFF'u64
            x0 := cast(u32) x
            x1 := cast(u8) x
            x2 := cast(u16) x
        }
    }

    @assert(G == 6)
}