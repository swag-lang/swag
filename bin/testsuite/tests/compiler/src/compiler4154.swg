func(N) titi()->N => true

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("", false)]
func(N) toto()->N
{
    aa := titi'N()
    return cast(N) aa
}

#test
{
    x := toto'bool()
    @assert(x == true)
}
