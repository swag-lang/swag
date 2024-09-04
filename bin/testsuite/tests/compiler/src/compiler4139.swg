#global testerror "Err0671"

#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #inject stmt
    }
}

#test
{
    var t: s32
    var x: s32
    toto() {
        var t: s32
        var x: s32
        @print(#offsetof(x))
    }
    @print(#offsetof(x))
}