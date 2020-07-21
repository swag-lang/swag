#[swag.printbc]
#test
{
    x := 5's64
    y := x == 5
    @print(cast(s64)y)
}