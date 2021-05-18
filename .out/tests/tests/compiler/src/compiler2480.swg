var G = 0
func panic(msg: string, loc: swag.CompilerSourceLocation)
{
    G += 1
}

#[swag.safety("overflow", true)]
#test
{
    {
        cxt := @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        x := 10'u8
        x += 255
        x += 255
    }

    @assert(G == 2)
}