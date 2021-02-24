#[swag.nocopy]
struct AA
{
}

struct(T) BB
{
}

impl BB
{
    #[swag.compileif(@typeof(T) != AA)]
    func opIndexAffect(using self, index: uint, value: T)
    {
        var x: T
        x = value
    }
}

#test
{
    var x: BB'AA
}