#[swag.printbc]
#test
{
    x := 8'f64
    x = -x
    @print(cast(s64)x)
}