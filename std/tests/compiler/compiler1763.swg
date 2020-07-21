#[swag.printbc]
#test
{
    x := 0xFFFFFFFF'u32
    y := x + 1
    @print(cast(f64)y)
}