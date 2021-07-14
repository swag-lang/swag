using Swag

var G = 0
func panic(msg: string, loc: Swag.CompilerSourceLocation)
{
    G += 1
}

#[safety("overflow", true)]
//#[PrintBc]
#test
{
    {
        cxt := @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        {
            var x: u8 = Swag.U8.Max
            x <<= 1
        }

        {
            var x: u8 = 0b01000000
            x <<= 4
        }

        {
            var x: u16 = Swag.U16.Max
            x <<= 1
        }

        {
            var x: u32 = Swag.U32.Max
            x <<= 1
        }

        {
            var x: u64 = Swag.U64.Max
            x <<= 1
            x = 1'u64 << 63
            x <<= 1
        }
    }

    @assert(G == 6)
}