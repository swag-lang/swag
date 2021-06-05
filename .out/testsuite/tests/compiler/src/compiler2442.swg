struct A
{
    x, y: u64
}

func toto()->{A, err: string}
{
    var result: retval
    result.err = "err"
    return result
}

//#[Swag.printbc]
#test
{
    (v, err) := toto()
    @assert(err == "err")
}