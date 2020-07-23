struct A
{
    x: *s64
}

#[swag.printbc]
#test
{
    var v: A
    var c: s64 = 666

    v.x = &c

    ptr := v.x
    @print(deref ptr)
}