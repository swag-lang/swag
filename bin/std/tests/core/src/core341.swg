using Core

interface I
{
    x: func(self)
}

func toto()
{
    var arr: Array'I
    arr[0].x()
}