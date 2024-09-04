#global testerror "Err0394"
//#global testerror "Err0602"

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