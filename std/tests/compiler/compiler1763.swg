#[swag.printbc]
#test
{
    x := 255'u8
    x += 1
    @print(cast(s64) x)
}