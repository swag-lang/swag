#[swag.printbc]
#test
{
    x := 15's64
    x &= 5
    @print(cast(s64)x)
}