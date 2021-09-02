interface ITF0
{
    toto: func(self)
}

interface ITF1
{
    toto: func(self)
}

struct AA
{
}

impl ITF0 for AA
{
    func toto(self)
    {
    }
}

impl ITF1 for AA
{
    func toto(self)
    {
    }
}

#test
{
    var x: AA
    x.ITF0.toto()
    x.ITF1.toto()
}