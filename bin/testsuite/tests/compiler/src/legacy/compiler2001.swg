#global testerror "Err0630"

struct(T) B
{
    xxx: *T
}

impl B
{
    #[Swag.Safety("", "", true)]
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