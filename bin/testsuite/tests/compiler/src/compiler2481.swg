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
            let x = -1's8
            let x0 = cast(u8) x
            let x1 = cast(u16) x
            let x2 = cast(u32) x
            let x3 = cast(u64) x
        }

        {
            let x = -1's16
            let x0 = cast(u8) x
            let x1 = cast(u16) x
            let x2 = cast(u32) x
            let x3 = cast(u64) x
        }

        {
            let x = -1's32
            let x0 = cast(u8) x
            let x1 = cast(u16) x
            let x2 = cast(u32) x
            let x3 = cast(u64) x
        }

        {
            let x = -1's64
            let x0 = cast(u8) x
            let x1 = cast(u16) x
            let x2 = cast(u32) x
            let x3 = cast(u64) x
        }

        {
            let x = Swag.S16.Max
            let x0 = cast(u8) x
        }

        {
            let x = Swag.S32.Max
            let x0 = cast(u8) x
            let x1 = cast(u16) x
        }

        {
            let x = Swag.S64.Max
            let x0 = cast(u8) x
            let x1 = cast(u16) x
            let x2 = cast(u32) x
        }
    }

    @assert(G == 28)
}