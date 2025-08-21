
#global testerror "Err0176"

struct B
{
    front: bool
}

impl B
{
    func toto(using me)
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