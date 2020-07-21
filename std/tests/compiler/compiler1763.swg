#[swag.printbc]
#test
{
    x := 10'f64
    x -= 4
    @print(cast(s64)x)
}