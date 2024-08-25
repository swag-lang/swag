#[Swag.Macro]
public func macro2365(stmt: code)
{
    #scope up
    for 10
    {
        #scope down
        #macro
        {
            #mixin #up stmt where { break = break in up; continue = break in down; }
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