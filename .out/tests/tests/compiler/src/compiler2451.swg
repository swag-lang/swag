struct A
{
    x: s32 = 666
    v: *~32 A
}

#assert @sizeof(A) == 8

//#[swag.printbc]
#test
{
    var x: A
    x.v = &x
    @assert(x.v.x == 666)
}