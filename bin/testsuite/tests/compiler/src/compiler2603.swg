#global testerror "Err0436"
//#global testerror "Err0662"

#[Swag.Macro]
func(ptr: bool) opVisitToto(x: string, stmt: code)
{
    for x: #mixin stmt
}
/*
#test
{
    foreach<Toto> "toto"
    {

    }
}*/