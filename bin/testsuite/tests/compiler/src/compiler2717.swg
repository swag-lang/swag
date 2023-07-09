//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
func toto()->s64
{
    var r = 666
    return r
}

#test
{
    @assert(toto() == 666)
}