#global testerror "Err0140"

#[Swag.Mixin]
func toto(stmt: code)
{
    loop 5
    {
        #mixin stmt { toto = titi; }
    }
}
