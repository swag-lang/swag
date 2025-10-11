interface ITF0
{
    func toto(me);
}

interface ITF1
{
    func toto(me);
}

struct AA {}

impl ITF0 for AA
{
    func impl toto(me) {}
}

impl ITF1 for AA
{
    func impl toto(me) {}
}

#test
{
    var x: AA
    x.ITF0.toto()
    x.ITF1.toto()
}
