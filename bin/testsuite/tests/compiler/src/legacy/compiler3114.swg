#[Swag.Strict]
{
    alias AA = *void
    alias BB = *void
}

#[Swag.Overload]
func test(x: AA) {}

#[Swag.Overload]
func test(x: BB) {}

func totoXXX()->AA
{
    var x: AA
    return x
}

#test
{
    let vvv = totoXXX()
    @assert(#nameof(#typeof(vvv)) == "AA")
    test(vvv)
}
