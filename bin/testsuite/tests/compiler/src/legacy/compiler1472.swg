struct S
{
    down:     s32 = 10
    up:       s32 = 20
}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisit(me, stmt: #code void)
    {
        for var v = me.down; v <= me.up; v += 1
        {
            #macro
            {
                var #alias0 = #up v
                #inject(#up stmt)
            }
        }
    }
}

#test
{
    var x:  S
    var cpt = 0
    foreach i in x
    {
        var xx = 666
        cpt += i
    }

    @assert(cpt == 165)
}
