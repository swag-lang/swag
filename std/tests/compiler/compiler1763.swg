#[swag.printbc]
#test
{
    x := 10'f64
    x = x / 0
    @print(cast(s64) x);
}