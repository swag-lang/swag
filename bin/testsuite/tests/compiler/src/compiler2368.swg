#global testerror "Err0337"
#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin #up stmt where { break = break in up; }
    }
}
