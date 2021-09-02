#[Swag.Inline]
func assert(v: bool)
{
}

struct S
{
}

impl S
{
    func titi(using const self)=>true
}

func tata(x: S)
{
    assert(x.titi())
}
