func(T, N: uint) dot(x, y: const [N] T)->T
{
    var result: T
    loop i: x
        result += x[i] * y[i]
    return result
}

func(T, N: uint) norm(a: const [N] T)->T
{
    return @sqrt(dot(a, a))
}

//#[Swag.PrintBc]
#test
{
    const X = @[1.5, 2]
    @assert(norm(X) == 2.5)
}