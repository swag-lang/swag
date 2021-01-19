struct A
{
    x, y: u64
}

alias GG = A

func toto()->{GG, string}
{
    var result: retval
    return result
}

//#[swag.printbc]
#test
{
    (v, err) := toto()
}