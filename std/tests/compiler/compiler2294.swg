struct AA
{
    x, y: s32
}

impl AA
{
    func titi(using self) => true
}

#[swag.inline]
func xx(b: bool)
{
}

func toto(all: *AA...)
{
    xx(all[0].titi())
}
