struct A
{
    x, y: s64
}

func titi(p: *A)
{
    p.x = 444
    p.y = 555
}

//#[Swag.printbc]
func toto()->A
{
    var result: retval
    titi(&result)
    return result
}

#test
{
    res := toto()
    @assert(res.x == 444)
    @assert(res.y == 555)
}