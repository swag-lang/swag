var y: s32 = add(10, x)
const x: s32 = add(12, 34)

#[swag.constexpr]
func add(a, b: s32) -> s32
{
    return a + b
}

#test
{
    @assert(y == 56)
    @assert(x == 46)
}