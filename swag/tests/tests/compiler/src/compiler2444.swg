#global testerror

struct A
{
    x: s64 = 666
    y: s64 = 555
}

#test
{
    var v: A
    alias err = v.x
    //@print(err)
}