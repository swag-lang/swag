#global testerror "Err0405"
//#global testerror "Err0614"

#[Swag.Macro]
func(ptr: bool) opVisitToto(x: string, stmt: code)
{
    for x: #inject stmt
}
/*
#test
{
    foreach<Toto> "toto"
    {

    }
}*/