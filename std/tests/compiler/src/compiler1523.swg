#module "compiler1523"
#unittest error

struct X
{
    x: s32
    y: bool   
}

impl X
{
    func toto(using const self)
    {
        x = 666
    }
}

func titi(str: X)
{
    str.toto()
}

#test
{
    var p: X
    titi(p)
}
