#[Swag.Mixin, Swag.CalleeReturn]
func mac(stmt: #block void)
{
    #inject stmt
}

func toto()->s32
{
    mac()
    {
        return 1
    }

    @assert(false)
}

#test
{
    let x = toto()
    @assert(x == 1)
}