func(T) toto(x: T...)->T
{
    var res: T
    visit c: x
        res += c
    return res
}

//#[Swag.printbc]
#test
{
    res := toto(10's32, 20, 30)
    @assert(res == 60)
    //res1 := toto(1.0'f32, 0.5, 3)
    //@assert(res1 == 4.5)
}