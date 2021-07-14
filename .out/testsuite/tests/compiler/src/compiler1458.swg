struct S
{
}

impl S
{
    #[Swag.macro, Swag.NoReturn]
    func(ptr: bool) opVisitBytes(using self, stmt: code)
    {
        label up
        loop 5
        {
            loop 5
            {
                #macro
                {
                    #mixin `stmt
                }
            }
        }
    }
}

#test
{
    var x: S

    cpt := 0
    visit(Bytes) x
    {
        cpt += 1
        //@print("aa\n")
        break up
    }

    @assert(cpt == 1)
}