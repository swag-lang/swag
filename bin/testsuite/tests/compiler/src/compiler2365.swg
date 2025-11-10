#[Swag.Macro]
public func macro2365(stmt: #code void)
{
    #scope(up)
    for 10
    {
        #scope(down)
        #macro
        {
            #inject(#up stmt, break = break to up, continue = break to down)
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
