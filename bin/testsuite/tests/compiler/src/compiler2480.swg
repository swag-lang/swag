var G = 0
func panic(msg: string, loc: Swag.SourceCodeLocation)
{
    G += 1
}

#[Swag.Safety("overflow", true)]
#[Swag.Safety("sanity", false)]
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