struct S
{
}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisitBytes(using self, stmt: code)
    {
        #scope up
        loop 5
        {
            loop 5
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
    visit,Bytes x
    {
        cpt += 1
        //@print("aa\n")
        break up
    }

    @assert(cpt == 1)
}