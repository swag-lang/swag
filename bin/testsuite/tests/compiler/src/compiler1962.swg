interface ITF0
{
    func toto(self);
}

interface ITF1
{
    func toto(self);
}

struct AA
{
}

impl ITF0 for AA
{
    func impl toto(self)
    {
    }
}

impl ITF1 for AA
{
    func impl toto(self)
    {
    }
}

#test
{
    var x: AA
    x.ITF0.toto()
    x.ITF1.toto()
}
