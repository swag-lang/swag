//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
func toto()->s64
{
    let r = 666
    return r
}

#test
{
    @assert(toto() == 666)
}