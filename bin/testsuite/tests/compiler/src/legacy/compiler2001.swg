#global testerror "Err0508"

struct(T) B
{
    xxx: *T
}

impl B
{
    #[Swag.Safety(.All, .All, true)]
    func toto(me)
    {
        :xxx = 0
    }
}

#test
{
    var y: B's32
    y.toto()
}