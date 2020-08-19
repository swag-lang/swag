#module "compiler1591"
#unittest error

struct B
{
    front: bool
}

impl B
{
    func toto(using self)
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