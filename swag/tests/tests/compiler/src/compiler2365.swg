#[swag.macro]
public func macro2365(stmt: code)
{
    label up
    loop 10
    {
        label down
        #macro
        {
            #mixin `stmt { break = break up; continue = break down; }
        }

        `g += 1
    }
}

#test
{
    g := 0
    macro2365()
    {
        break
    }

    @assert(g == 0)

    macro2365()
    {
        continue
    }

    @assert(g == 10)
}