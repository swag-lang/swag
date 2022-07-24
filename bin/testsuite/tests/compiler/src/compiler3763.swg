//#global skip

func toto(x: s8)
{
    @assert(x == 11)
}

#[Swag.Optim("", false)]
#test
{
    c := 1's8
    d := 10's8
    toto(c + d)
}