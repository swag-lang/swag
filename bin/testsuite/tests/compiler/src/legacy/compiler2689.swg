#global testerror "Err0365"

struct AA
{
    x:   s64
}

impl AA
{
    #[Swag.CalleeReturn]
    func opAffect(me, p: s64)
    {
        x = p
    }
}

func toto() {}
