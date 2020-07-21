#[swag.printbc]
#test
{
    x := 8's64
    x ^= 3
    @print(cast(s64)x)
}