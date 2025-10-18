#global testerror "Err0093"

#[Swag.Macro]
func toto(stmt: #code void)
{
    #macro
    {
        #inject(stmt)
    }
}

#test
{
    var t: s32
    var x: s32
    toto()
    {
        var t: s32
        var x: s32
        @print(#offsetof(x))
    }
    @print(#offsetof(x))
}
