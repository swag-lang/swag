#global testerror "Err0408"
//#global testerror "Err0701"

#[Swag.Macro]
func(ptr: bool) opVisitToto(x: string, stmt: code)
{
    loop x: #mixin stmt
}
/*
#test
{
    visit<Toto> "toto"
    {

    }
}*/