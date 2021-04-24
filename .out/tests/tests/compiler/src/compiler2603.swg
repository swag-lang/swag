#global testerror

#[swag.macro]
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