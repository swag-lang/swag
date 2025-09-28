#global testerror "Err0713"

interface AA
{
    //#[Swag.Discardable]
    func fb(me)->s32;
}

struct BB
{

}

impl AA for BB
{
    func impl fb(me)->s32
    {
        return 666
    }
}

#test
{
    var x: BB
    var itf = cast(AA) x
    itf.fb()
}