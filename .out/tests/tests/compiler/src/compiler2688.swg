#global testerror

struct AA
{
    x: s64
}

impl AA
{
    #[swag.property]
    func opAffect(using self, p: s64)
    {
        x = p
    }
}

func toto()
{
}