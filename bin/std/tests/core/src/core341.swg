using Core

interface I
{
    func x(self);
}

func toto()
{
    var arr: Array'I
    arr[0].x()
}
