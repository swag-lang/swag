//#global skip
var g = 6'u64

#[Swag.Optimize("", false), Swag.Safety("", "", false)]
#test
{
    let a = @countof(g)
    @assert(a == 6)
}
