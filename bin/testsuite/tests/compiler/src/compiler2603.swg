#global testerror "Err0454"
//#global testerror "Err0699"

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