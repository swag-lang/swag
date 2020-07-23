#[swag.printbc]
#test
{
    x := 10'f32
    x *= 2
    @print(cast(s64) x);
}