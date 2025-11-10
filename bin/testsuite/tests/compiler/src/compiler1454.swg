struct S {}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisit(me, stmt: #code void)
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
    foreach x
    {
        cpt += 1
    }

    @assert(cpt == 10)
}
