#global testerror "toto"

#[Swag.Macro]
func toto(v: u8)
{
    #ast
    {
        @compilererror("toto", #location(v))
        return ""
    }
}

#[Swag.Macro]
func titi(v: u8)
{
    toto(v)
}

#test
{
    var x = 1'u8
    titi(x)
}