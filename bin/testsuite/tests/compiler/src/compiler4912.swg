#global testerror "Err0117"

#[Swag.Mixin]
func toto(stmt: code)
{
    for 5
    {
        #inject stmt => { toto = titi; }
    }
}
