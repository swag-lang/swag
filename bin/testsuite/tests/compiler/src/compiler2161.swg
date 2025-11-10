#[Swag.Inline]
func assert(v: bool) {}

struct S {}

impl S
{
    func titi(const me) => true
}

func tata(x: S)
{
    assert(x.titi())
}
