//#global testerror

struct A
{
}

impl A
{
    mtd titi()
    {
    }

    #[Swag.Macro]
    func toto(using self, stmt: code)
    {
        titi()
    }
}

#test
{
    var v: A
    v.toto()
    {
    }
}