#global testerror "Err0190"
using Core

interface I
{
    x: func(me)
}

func toto()
{
    var arr: Array's32
    arr[0].x()
}