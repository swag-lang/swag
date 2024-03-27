#[Swag.Strict]
{
    typealias AA = *void
    typealias BB = *void
}

#[Swag.Overload]
func test(x: AA)
{
}

#[Swag.Overload]
func test(x: BB)
{
}

#test
{
    var vvv: AA
    test(vvv)
}
