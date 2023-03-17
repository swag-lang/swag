#[Swag.Macro]
public func macro2365(stmt: code)
{
    #scope Up
    loop 10
    {
        #scope Down
        #macro
        {
            #mixin stmt { break = break Up; continue = break Down; }
        }

        #up g += 1
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