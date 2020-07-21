#[swag.printbc]
#test
{
    x := 5's64
    x |= 10
    @print(cast(s64)x)
}