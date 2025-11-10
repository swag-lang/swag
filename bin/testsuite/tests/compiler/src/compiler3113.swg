#[Swag.Strict]
{
    alias AA = *void
    alias BB = *void
}

#[Swag.Overload]
func test(x: AA) {}

#[Swag.Overload]
func test(x: BB) {}

#test
{
    var vvv: AA
    test(vvv)
}
