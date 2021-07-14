#global testerrors

#[Swag.Macro]
func(ptr: bool) opVisitToto(x: string, stmt: code)
{
    loop x #mixin stmt
}

#test
{
    visit(Toto) "toto"
    {

    }
}