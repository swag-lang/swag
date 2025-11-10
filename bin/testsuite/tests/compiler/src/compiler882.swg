//#[Swag.PrintBc]
#test
{
    let a = true ? "1" : "10"
    @assert(a == "1")
    let b = false ? "1" : "10"
    @assert(b == "10")
}
