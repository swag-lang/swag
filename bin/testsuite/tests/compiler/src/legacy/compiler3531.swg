func(T, N: u64) dot(x, y: const [N] T)->T
{
    var result: T
    for i in x do
        result += x[i] * y[i]
    return result
}

func(T, N: u64) norm(a: const [N] T)->T
{
    return @sqrt(dot(a, a))
}

//#[Swag.PrintBc]
#test
{
    const X = [1.5, 2]
    @assert(norm(X) == 2.5)
}
