public struct AA2072
{
}

public impl AA2072
{
    enum ENUM
    {
        X
        Y
    }

    func toto(using self)
    {
        using self.ENUM
        y := X
    }
}

#test
{
    var x: AA2072
    x.toto()
}
