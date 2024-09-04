#global testerror "Err0296"
#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #inject #up stmt => { break = break to up; }
    }
}
