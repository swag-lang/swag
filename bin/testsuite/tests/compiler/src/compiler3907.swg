//#[Swag.PrintBc]
func toto(x: const [..] s32)
{
    let a = &x[0]
    @assert(#typeof(a).name == "const [*] s32")
    @assert(#typeof(a) == const [*] s32)
}

#test
{
    var x = [1]
    toto(x)
}
