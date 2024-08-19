#global testerror "Err0405"
//#global testerror "Err0698"

#[Swag.Macro]
func(ptr: bool) opVisitToto(x: string, stmt: code)
{
    loop x: #mixin stmt
}
/*
#test
{
    visit(Toto) "toto"
    {

    }
}*/