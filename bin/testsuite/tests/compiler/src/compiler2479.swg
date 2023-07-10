var G = 0
func panic(msg: string, loc: Swag.SourceCodeLocation)
{
    G += 1
}

//#[Swag.PrintBc]
#[Swag.Safety("overflow", true)]
#[Swag.Optim("", false)]
#test
{
    {
        let cxt = @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            let x = 0xFFFF'u16
            let x0 = cast(u8) x
        }

        {
            let x = 0xFFFFFFFF'u32
            let x0 = cast(u8) x
            let x1 = cast(u16) x
        }

        {
            let x = 0xFFFFFFFF_FFFFFFFF'u64
            let x0 = cast(u32) x
            let x1 = cast(u8) x
            let x2 = cast(u16) x
        }
    }

    @assert(G == 6)
}