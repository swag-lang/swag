#[Swag.Strict]
{
    typealias AA = *void
    typealias BB = *void
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
    let vvv = totoXXX()
    @assert(@nameof(@typeof(vvv)) == "AA")
    test(vvv)
}