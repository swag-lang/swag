struct A
{
    x, y: u64
}

func toto()->{a: A, err: string}
{
    var result: retval
    result.err = "err"
    return result
}

//#[Swag.PrintBc]
#test
{
    var (v, err) = toto()
    @assert(err == "err")
}