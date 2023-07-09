#[Swag.Strict]
{
    alias AA = *void
    alias BB = *void
}

func test(x: AA)
{
}

func test(x: BB)
{
}

func totoXXX()->AA
{
    var x: AA
    return x
}

#test
{
    var vvv = totoXXX()
    @assert(@nameof(@typeof(vvv)) == "AA")
    test(vvv)
}