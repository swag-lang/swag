#global testerror "Err0389"
//#global testerror "Err0682"

#[Swag.Macro]
func(ptr: bool) opVisitToto(x: string, stmt: code)
{
    loop x do #mixin stmt
}
/*
#test
{
    visit(Toto) "toto"
    {

    }
}*/