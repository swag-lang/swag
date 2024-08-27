//#global skip
var g = 6'u64

func toto()->&u64
{
    return &g
}

#[Swag.Optimize("", false)]
#[Swag.Safety("", false)]
#test
{
    let a: u32 = cast() toto()
    @assert(a == 6)
}