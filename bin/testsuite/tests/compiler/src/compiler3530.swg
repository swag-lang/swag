struct(T, N: u64) StaticArray
{
    buffer: [N] T
}

impl StaticArray
{
    mtd const opCount()->u64       => StaticArray.N
    mtd const opIndex(idx: u64)->T => me.buffer[idx]
}

alias Vec2 = StaticArray'(f32, 2)

func(T) dot(x, y: T)->f32
{
    var result: f32
    for i in x do
        result += x[i] * y[i]
    return result
}

func(T) norm(a: T)->f32
{
    return @sqrt(dot(a, a))
}

//#[Swag.PrintBc]
#test
{
    const x = Vec2{[1.5, 2]}
    @assert(norm(x) == 2.5)
}
