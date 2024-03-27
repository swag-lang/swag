using test_foreign

func titi(b: u64, x: string, d: u64)->u64
{
    @assert(b == 3)
    @assert(x == "toto")
    @assert(d == 666)
    return 666
}

#test
{
    var res = call42("toto", &titi)
    @assert(res == 666)
}