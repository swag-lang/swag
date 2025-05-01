#global testerror "Err0306"
#[Swag.Macro]
func toto(stmt: #block void)
{
    #macro
    {
        #inject #up stmt => { break = break to up; }
    }
}
