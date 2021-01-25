struct A
{
    x: s32 = 666

    #[swag.relative(4)]
    v: *A
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