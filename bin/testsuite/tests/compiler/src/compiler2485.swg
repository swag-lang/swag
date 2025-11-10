/*using Swag

var G = 0
func panic(msg: string, loc: Swag.SourceCodeLocation)
{
    G += 1
}

#[Safety(.All, .Overflow, true)]
#[Optimize(.All, false)]
//#[PrintBc]
#test
{
    {
        let cxt = @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            var x: s8 = Swag.S8.Max
            x <<= 1
        }

        {
            var x: s8 = 0b01000000
            x <<= 4
        }

        {
            var x: s16 = Swag.S16.Max
            x <<= 1
        }

        {
            var x: s32 = Swag.S32.Max
            x <<= 1
        }

        {
            var x: s64 = Swag.S64.Max
            x <<= 1
            x = 1's64 << 63
            x <<= 1
        }
    }

    @assert(G == 6)
}*/
