struct A
{
    x: [17] bool
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