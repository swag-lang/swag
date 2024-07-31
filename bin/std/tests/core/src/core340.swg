#global testerror "Err0294"
using Core

interface I
{
    x: func(self)
}

func toto()
{
    var arr: Array's32
    arr[0].x()
}