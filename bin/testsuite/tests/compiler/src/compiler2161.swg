#[Swag.Inline]
func assert(v: bool)
{
}

struct S
{
}

impl S
{
    func titi(using const me)=>true
}

func tata(x: S)
{
    assert(x.titi())
}
