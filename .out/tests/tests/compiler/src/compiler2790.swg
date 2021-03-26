#global testerror

interface AA
{
    //#[swag.autodiscard]
    fb: func(self)->s32
}

struct BB
{

}

impl AA for BB
{
    func fb(self)->s32
    {
        return 666
    }
}

#test
{
    var x: BB
    itf := cast(AA) x
    itf.fb()
}