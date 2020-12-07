#[swag.inline]
func assert(v: bool)
{
}

struct S
{
}

impl S
{
    func titi(using self)=>true
}

func tata(x: S)
{
    assert(x.titi())
}
