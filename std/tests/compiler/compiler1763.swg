#[swag.printbc]
#test
{
    x := 66'u32
    b := x * 2
    @print(cast(s64) b)
}