//#[Swag.PrintBc]
func(T, N: uint) norm(a: const [N] T)
{
    var arr: [N] s32
    @assert(N == 2)
    @assert(T == f32)

}

//#[Swag.PrintBc]
#test
{
    const X = @[1.5, 2]
    norm(X)
}