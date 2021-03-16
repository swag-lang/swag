//#[swag.printbc]
//#[swag.optim("", false)]
func toto()->s64
{
    r := 666
    return r
}

#test
{
    @assert(toto() == 666)
}