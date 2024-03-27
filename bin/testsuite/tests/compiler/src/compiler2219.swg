func(T) toto(x: T...)->T
{
    var res: T
    visit c: x do
        res += c
    return res
}

//#[Swag.PrintBc]
#test
{
    let res = toto(10's32, 20, 30)
    @assert(res == 60)
}
