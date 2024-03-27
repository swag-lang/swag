#global testerror "Err0085"

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
        #validif @typeof(T) != AA
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