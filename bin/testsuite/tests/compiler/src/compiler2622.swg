#global testerror "Err0060"

#[Swag.NoCopy]
struct AA
{
}

struct(T) BB
{
}

impl BB
{
    //#[Swag.compileif(@typeof(T) != AA)]
    func opIndexAffect(using self, index: u64, value: T)
        where @typeof(T) != AA
    {
        var x: T
        x = value
    }
}

#test
{
    var x: BB'AA
    var y: AA
    x[0] = y
}