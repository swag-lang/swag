//#[Swag.PrintBc]
#test
{
    let c = 666
    let d = [1, c, 2]

    @assert(d[0] == 1)
    @assert(d[2] == 2)
    @assert(d[1] == 666)
    //@print("这是什么");
}
