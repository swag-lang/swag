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
            var x: u8 = swag.U8.Max
            x <<= 1
        }

        {
            var x: u8 = 0b01000000
            x <<= 4
        }

        {
            var x: u16 = swag.U16.Max
            x <<= 1
        }

        {
            var x: u32 = swag.U32.Max
            x <<= 1
        }

        {
            var x: u64 = swag.U64.Max
            x <<= 1
            x = 1'u64 << 63
            x <<= 1
        }
    }

    @assert(G == 6)
}