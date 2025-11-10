func(T) toto(x: T...)->T
{
    var res: T
    foreach c in x do
        res += c
    return res
}

//#[Swag.PrintBc]
#test
{
    let res = toto(10's32, 20, 30)
    @assert(res == 60)
    //var res1 = toto(1.0'f32, 0.5, 3)
    //@assert(res1 == 4.5)
}
