using Core

interface I
{
    func x(me);
}

func toto()
{
    var arr: Array'I
    arr[0].x()
}
