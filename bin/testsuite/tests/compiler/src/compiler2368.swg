#global testerror "Err0316"
#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin #up stmt where { break = break in up; }
    }
}
