struct S
{
}

impl S
{
    #[swag.macro]
    func opVisitBytes(using self, stmt: code)
    {
        up:
        loop 5
        {
            loop 5
            {
                #macro
                {
                    #mixin `stmt
                }
            }
        }
    }
}

#test
{
    var x: S
    
    cpt := 0
    visit(Bytes) x
    {
        cpt += 1
        //@print("aa\n")
        break up
    }
    
    @assert(cpt == 1)
}