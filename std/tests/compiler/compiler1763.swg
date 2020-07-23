#[swag.printbc]
#test
{
    x := 10'f32
    x = x / 0
    @print(cast(s64) x);
}