func(N) titi()->N => true

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, false)]
func(N) toto()->N
{
    let aa = titi'N()
    return cast(N) aa
}

#test
{
    let x = toto'bool()
    @assert(x == true)
}
