#global testerror "Err0120"

#[Swag.Mixin]
func toto(stmt: #block void)
{
    for 5
    {
        #inject stmt => { toto = titi; }
    }
}
