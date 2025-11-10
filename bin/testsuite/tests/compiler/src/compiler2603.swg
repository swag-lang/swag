#global testerror "Err0320"
//#global testerror "Err0690"

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
