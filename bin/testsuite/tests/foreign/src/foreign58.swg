#[Swag.Macro]
public func parse58(stmt: code)
{
    for
    {
        #macro
        {
            #mixin #up stmt
        }
    }

    return
}

#[Swag.Inline]
public func toto58()
{
    var countTot = 0
    parse58()
    {
        countTot += 1
    }

    @assert(countTot == 4)
}
