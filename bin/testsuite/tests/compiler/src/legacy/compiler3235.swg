struct A {}

impl A
{
    #[Swag.Inline]
    mtd const isValid() => true
}

func toto(x: A) {}

func titi(x: A = {})
{
    toto(x.isValid() ? x : {})
}
