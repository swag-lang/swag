#[swag.printbc]
#test
{
    x := 66'f32
    b := cast(s64)(x * 2)
    @print(b)
}