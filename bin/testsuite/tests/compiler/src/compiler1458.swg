struct S
{
}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisitBytes(using self, stmt: #code void)
    {
        #scope(up)
        for 5
        {
            for 5
            {
                #macro
                {
                    #inject(#up stmt)
                }
            }
        }
    }
}

#test
{
    var x: S

    var cpt = 0
    foreach #Bytes x
    {
        cpt += 1
        //@print("aa\n")
        break to up
    }

    @assert(cpt == 1)
}