func(N) titi()->N => true

//#[Swag.PrintBc]
#[Swag.Optimize("", false), Swag.Safety("", false)]
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
