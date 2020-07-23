struct A
{
    x: s64
    y: s64
}

#[swag.printbc]
func toto()->A
{
    return {10, 20}
}

#[swag.printbc]
#test
{
    res := toto()
    @print(res.x)
}