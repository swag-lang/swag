/*using Swag

var G = 0
func panic(msg: string, loc: Swag.SourceCodeLocation)
{
    G += 1
}

#[Safety("sanity", false)]
#[Safety("overflow", true)]
#[Optimize("", false)]
//#[PrintBc]
#test
{
    {
        let cxt = @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            var x: u8 = 1
            x >>= 1
        }

        {
            var x: u16 = 1
            x >>= 1
        }

        {
            var x: u32 = 2
            x >>= 2
        }

        {
            var x: u64 = 4
            x >>= 4
        }
    }

    @assert(G == 4)
}*/