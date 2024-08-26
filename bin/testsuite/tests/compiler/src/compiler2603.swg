#global testerror "Err0430"
//#global testerror "Err0655"

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