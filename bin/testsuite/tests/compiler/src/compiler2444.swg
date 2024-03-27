#global testerror "Err0163"

struct A
{
    x:     s64 = 666
    y:     s64 = 555
}

#test
{
    var v: A
    namealias err = v.x
}
