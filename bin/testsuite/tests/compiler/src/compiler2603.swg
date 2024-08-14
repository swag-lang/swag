#global testerror "Err0396"
//#global testerror "Err0690"

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