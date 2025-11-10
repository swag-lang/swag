//#global skip
var g = 6'u64

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, false)]
#test
{
    let a = @countof(g)
    @assert(a == 6)
}
