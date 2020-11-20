struct A
{
    x: [16] bool
    y: bool = true
}

func titi()->A
{
    var result: retval
    return result
}

var G: A

#test
{
    G = titi()
}