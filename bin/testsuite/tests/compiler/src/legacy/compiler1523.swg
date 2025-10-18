#global testerror "Err0126"

struct X
{
    x:     s32
    y:     bool
}

impl X
{
    func toto(const me)
    {
        me.x = 666
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
