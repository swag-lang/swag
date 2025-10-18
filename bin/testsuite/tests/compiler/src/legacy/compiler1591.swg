#global testerror "Err0066"

struct B
{
    front: bool
}

impl B
{
    func toto(me)
    {
        var x: s32
        x.front = 666
    }
}

#test
{
    var arr: B
    arr.toto()
}
