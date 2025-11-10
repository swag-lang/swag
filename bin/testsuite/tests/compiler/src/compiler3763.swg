//#global skip

func toto(x: s8)
{
    @assert(x == 11)
}

#[Swag.Optimize(.All, false)]
#test
{
    let c = 1's8
    let d = 10's8
    toto(c + d)
}
