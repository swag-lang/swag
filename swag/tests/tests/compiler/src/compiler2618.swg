#[swag.nocopy]
struct AA
{
}

struct(T) BB
{
}

impl BB
{
    //#[swag.compileif(@typeof(T) != AA)]
    func opIndexAffect(using self, index: uint, value: T)
        #selectif @typeof(T) != AA
    {
        var x: T
        x = value
    }
}

#test
{
    var x: BB'AA
    /*var y: AA
    x[0] = y*/
}