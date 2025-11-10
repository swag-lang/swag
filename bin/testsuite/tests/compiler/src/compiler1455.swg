struct S {}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisitBytes(me, stmt: #code void)
    {
        for 10
        {
            #macro
            {
                #inject(#up stmt)
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
    }

    @assert(cpt == 10)
}
