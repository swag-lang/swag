#global testerror "Err0120"

#[Swag.Mixin]
func toto(stmt: #code void)
{
    for 5
    {
        #inject(stmt) => { toto = titi; }
    }
}
