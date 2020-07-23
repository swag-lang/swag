#[swag.printbc]
#test
{
    x := 10'u32
    x /= 3
    @print(cast(s64) x);
}