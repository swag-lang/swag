#global testerror "Err0160"

struct A
{
    x: s64 = 666
    y: s64 = 555
}

#test
{
    var v: A
    namealias err = v.x
    //@print(err)
}