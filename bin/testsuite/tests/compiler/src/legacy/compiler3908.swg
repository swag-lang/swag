//#[Swag.PrintBc]
func toto(x: [..] s32)
{
    let a = &x[0]
    @assert(#typeof(a).name == "[*] s32")
    @assert(#typeof(a) == #type [*] s32)
}

#test
{
    var x = [1]
    toto(x)
}
