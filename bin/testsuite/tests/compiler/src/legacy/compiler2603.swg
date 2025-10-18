#global testerror "Err0280"
//#global testerror "Err0320"

#[Swag.Macro]
func(ptr: bool) opVisitToto(x: string, stmt: #code void)
{
    for x do
        #inject(stmt)
}
/*
#test
{
    foreach<Toto> "toto"
    {

    }
}*/
