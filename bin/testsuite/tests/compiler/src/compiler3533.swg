//#[Swag.PrintBc]
func(N: u64) norm(a: const [N] f32)
{
    var arr: [N] s32
    @assert(N == 2)
    @assert(a[0] == 1.5)
}

//#[Swag.PrintBc]
#test
{
    const X = [1.5, 2]
    norm(X)
}
