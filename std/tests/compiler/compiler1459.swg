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
                #macro //(break = break up)
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
        break
    }
    
    //@assert(cpt == 1)
}