#global testerror "Err0171"
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