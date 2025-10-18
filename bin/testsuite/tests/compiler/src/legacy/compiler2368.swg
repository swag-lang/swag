#global testerror "Err0505"
#[Swag.Macro]
func toto(stmt: #code void)
{
    #macro
    {
        #inject(#up stmt, break = break to up)
    }
}
