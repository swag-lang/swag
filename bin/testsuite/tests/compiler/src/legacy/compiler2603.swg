#global testerror "Err0406"
//#global testerror "Err0614"

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
