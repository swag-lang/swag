struct A
{
    x: s32 = 666
    v: *~4 A
}

#assert @sizeof(A) == 8

//#[swag.printbc]
#test
{
    var x: A
    x.v = &x

    @assert(x.v.x == 666)
    v := x.v
    @assert(v.x == 666)
}