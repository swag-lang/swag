using Core

interface I
{
    func x(me);
}

func toto()
{
    let arr: Array'I
    arr[0].x()
}
