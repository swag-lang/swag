#[swag.printbc]
#test
{
    x := 8's64
    x = ~x
    @print(cast(s64)x)
}