struct S {}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisitBytes(me, stmt: #code void)
    {
        #scope(up)
        for 5
        {
            for 5
            {
                #macro
                {
                    #inject(#up stmt, break = break to up)
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
        //@print("AAA\n")
        cpt += 1
        break
    }

    @assert(cpt == 1)
}
