#global testerror "Err0339"
#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #inject #up stmt where { break = break in up; }
    }
}
