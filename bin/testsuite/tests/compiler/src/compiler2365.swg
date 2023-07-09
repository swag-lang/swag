#[Swag.Macro]
public func macro2365(stmt: code)
{
    #scope up
    loop 10
    {
        #scope down
        #macro
        {
            #mixin #up stmt { break = break up; continue = break down; }
        }

        #up g += 1
    }
}

#test
{
    var g = 0
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