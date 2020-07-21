#[swag.printbc]
#test
{
    x := 66'u8
    y := cast(u64) x
    @print(cast(f64)y)
}