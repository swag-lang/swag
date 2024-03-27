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
                    #mixin #up stmt { break = break up; }
                }
            }
        }
    }
}

#test
{
    var x: S

    var cpt = 0
    visit:Bytes x
    {
        //@print("AAA\n")
        cpt += 1
        break
    }

    @assert(cpt == 1)
}
