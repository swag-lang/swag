#[swag.printbc]
#test
{
    {
        x := 1'u8
        @assert(cast(f64) x == 1)
        x += 1
        @assert(cast(f64) x == 2)
    }
}