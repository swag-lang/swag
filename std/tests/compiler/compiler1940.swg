//#module "compiler1939"
//#unittest error

struct(T) B
{
    xxx: *T
}

impl B
{
    func toto(using self)
    {
        xxx[0] = 0
    }
}

#test
{
    var y: B's32
    y.toto()
}