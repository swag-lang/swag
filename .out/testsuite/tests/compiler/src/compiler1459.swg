struct S
{
}

impl S
{
    #[Swag.macro, Swag.NoReturn]
    func(ptr: bool) opVisitBytes(using self, stmt: code)
    {
        label up
        loop 5
        {
            loop 5
            {
                #macro
                {
                    #mixin `stmt { break = break up; }
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