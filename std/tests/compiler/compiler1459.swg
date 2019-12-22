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
                    #mixin(break = break up) `stmt
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
        //@print("AAA\n")
        cpt += 1
        break
    }
    
    @assert(cpt == 1)
}