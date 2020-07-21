#[swag.printbc]
#test
{
    x := 255'f32
    b := cast(s8)x
    @print(cast(s64)b)
}