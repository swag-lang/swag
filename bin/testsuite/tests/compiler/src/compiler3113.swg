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

#test
{
    var vvv: AA
    test(vvv)
}