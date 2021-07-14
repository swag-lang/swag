struct S
{
}

impl S
{
    #[Swag.Macro, Swag.NoReturn]
    func(ptr: bool) opVisitBytes(using self, stmt: code)
    {
        loop 10
        {
            #macro
            {
                #mixin `stmt
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
    }

    @assert(cpt == 10)
}