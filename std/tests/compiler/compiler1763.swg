#[swag.printbc]
#test
{
    x := 15's64
    x = x & 2
    @print(cast(s64)x)
}