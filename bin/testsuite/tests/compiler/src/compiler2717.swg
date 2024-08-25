//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
func toto()->s64
{
    let r = 666
    return r
}

#test
{
    @assert(toto() == 666)
}