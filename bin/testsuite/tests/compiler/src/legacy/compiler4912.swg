#global testerror "Err0119"

#[Swag.Mixin]
func toto(stmt: #code void)
{
    for 5
    {
        #inject(stmt, toto = titi)
    }
}
