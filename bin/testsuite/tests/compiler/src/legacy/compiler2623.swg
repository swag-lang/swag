#global testerror "Err0078"

#[Swag.NoCopy]
struct AA
{
}

struct(T) BB
{
}

impl BB
{
    //#[Swag.compileif(#typeof(T) != AA)]
    func opIndexAffect(me, index: u64, value: T)
        where #typeof(T) != AA
    {
        var x: T
        x = value
    }
}

#test
{
    {
        var x: BB's32
        var y: s32
        x[0] = y
    }

    {
        var x: BB'AA
        var y: AA
        x[0] = y
    }
}