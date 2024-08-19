#global testerror "Err0369"

struct AA
{
    x: s64
}

impl AA
{
    #[Swag.CalleeReturn]
    func opAffect(using self, p: s64)
    {
        x = p
    }
}

func toto()
{
}