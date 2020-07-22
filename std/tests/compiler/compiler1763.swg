#[swag.printbc]
#test
{
    x := 5'u8
    y := cast(bool)x
    @print(cast(s64)y)
}