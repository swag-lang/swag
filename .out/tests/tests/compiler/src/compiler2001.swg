
#global testerror

struct(T) B
{
    xxx: *T
}

impl B
{
    #[Swag.safety("", true)]
    func toto(using self)
    {
        :xxx = 0
    }
}

#test
{
    var y: B's32
    y.toto()
}