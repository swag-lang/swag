#global testerror

struct AA
{
    x: s64
}

impl AA
{
    #[Swag.NoReturn]
    func opAffect(using self, p: s64)
    {
        x = p
    }
}

func toto()
{
}