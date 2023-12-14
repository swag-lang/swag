struct S
{
}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisitBytes(using self, stmt: code)
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
    visit,Bytes x
    {
        cpt += 1
        //@print("aa\n")
    }

    @assert(cpt == 10)
}