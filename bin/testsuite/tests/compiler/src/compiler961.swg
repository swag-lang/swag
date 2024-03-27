//#[Swag.PrintBc]
#test
{
    var t: [2] {rank: s32, value: f64}
    var t1 = t[0]
    t1.rank = 666
    @assert(t1.rank == 666)
}
