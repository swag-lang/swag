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

//#[swag.printbc]
#test
{
    (v, err) := toto()
    @assert(err == "err")
}