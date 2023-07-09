struct S
{
}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        loop 10
        {
            #macro
            {
                #mixin #up stmt
            }
        }
    }
}

#test
{
    var x: S

    var cpt = 0
    visit x
    {
        cpt += 1
    }

    @assert(cpt == 10)
}