#global testerror "Err0749"

interface AA
{
    //#[Swag.Discardable]
    func fb(self)->s32;
}

struct BB
{

}

impl AA for BB
{
    func impl fb(self)->s32
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