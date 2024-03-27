//#[Swag.PrintBc]
#test
{
    let a = "toto"

    let start = 1'u32
    let end = 2'u32
    let c = a[start..end]
    @assert(c == "ot")
}