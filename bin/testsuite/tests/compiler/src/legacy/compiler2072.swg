public struct AA2072 {}

public impl AA2072
{
    enum ENUM
    {
        X
        Y
    }

    func toto(me)
    {
        var y = ENUM.X
    }
}

#test
{
    var x: AA2072
    x.toto()
}
