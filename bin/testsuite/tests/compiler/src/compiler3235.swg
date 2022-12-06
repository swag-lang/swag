struct A
{
}

impl A
{
    #[Swag.Inline]
    mtdc isValid() => true
}

func toto(x: A)
{
}

func titi(x: A = {})
{
    toto(x.isValid() ? x : {})
}