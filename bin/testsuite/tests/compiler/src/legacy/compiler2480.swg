var G = 0
func panic(msg: string, loc: Swag.SourceCodeLocation)
{
    G += 1
}

#[Swag.Safety(.All, .Overflow, true), Swag.Sanity(false)]
#test
{
    {
        let cxt = @getcontext()
        cxt.panic = &panic
        defer cxt.panic = null

        var x = 10'u8
        x += 255
        x += 255
    }

    @assert(G == 2)
}
