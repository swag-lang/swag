struct S
{
}

impl S
{
    #[swag.macro, swag.noreturn]
    func(ptr: bool) opVisit(using self, stmt: code)
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
    visit x
    {
        cpt += 1
    }
    
    @assert(cpt == 10)
}