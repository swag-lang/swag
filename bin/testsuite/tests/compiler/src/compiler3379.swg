#[Swag.Macro]
func titi(stmt: code)
{
    #inject stmt
}

func toto()->{idx: s32, sep: bool}
{
    titi()
    {
        return {10, false}
    }
}

#test
{
    let (idx, sep) = toto()
    @assert(idx == 10)
    @assert(sep == false)
}