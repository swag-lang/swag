#module "compiler2001"
#unittest error

struct(T) B
{
    xxx: *T
}

impl B
{
    //#[swag.printbc]
    func toto(using self)
    {
        deref xxx = 0
    }
}

#test
{
    var y: B's32
    y.toto()
}