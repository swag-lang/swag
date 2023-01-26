#[Swag.Macro, Swag.Discardable]
public func parse58(stmt: code)->bool
{
    #macro
    {
        #mixin stmt
    }

    return true
}

#[Swag.Inline]
public func toto58()
{
    countTot := 0
    while parse58()
    {
        countTot += 1
    }

    @assert(countTot == 4)
}
