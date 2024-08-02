#global testerror "Err0137"

#[Swag.Mixin]
func toto(stmt: code)
{
    loop 5
    {
        #mixin stmt { toto = titi; }
    }
}
