#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin #up stmt
    }
}

func titi()->s32
{
    toto()
    {
        return 666
    }

    return 0
}

#test
{
    var x = titi()
    @assert(x == 666)
}