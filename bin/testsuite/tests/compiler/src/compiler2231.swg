struct A
{
    x = 1
}

alias TOTO = A

//#[Swag.PrintBc]
func toto1()->[2] TOTO
{
    var result: retval
    return result
}

#test
{
    var res = toto1()
    @assert(res[0].x == 1)
    @assert(res[1].x == 1)
}