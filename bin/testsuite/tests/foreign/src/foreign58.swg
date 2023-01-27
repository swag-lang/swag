#[Swag.Macro]
public func parse58(stmt: code)
{
    loop
    {
        #macro
        {
            #mixin stmt
        }
    }

    return
}

#[Swag.Inline]
public func toto58()
{
    countTot := 0
    parse58()
    {
        countTot += 1
    }

    @assert(countTot == 4)
}
