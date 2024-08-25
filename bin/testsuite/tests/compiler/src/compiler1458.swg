struct S
{
}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisitBytes(using self, stmt: code)
    {
        #scope up
        for 5
        {
            for 5
            {
                #macro
                {
                    #mixin #up stmt
                }
            }
        }
    }
}

#test
{
    var x: S

    var cpt = 0
    foreach<Bytes> x
    {
        cpt += 1
        //@print("aa\n")
        break in up
    }

    @assert(cpt == 1)
}