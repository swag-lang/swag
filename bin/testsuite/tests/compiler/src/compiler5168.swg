#test
{
    enum BasicErrors
    {
        FailedToLoad
        FailedToSave
    }

    enum MyErrors
    {
        using BasicErrors // The enum 'BasicErrors' is nested inside
        NotFound = 100
    }

    const E0: MyErrors = MyErrors.BasicErrors.FailedToLoad
    const E1: BasicErrors = BasicErrors.FailedToLoad

    func toto(err: MyErrors)
    {
        @assert(err == BasicErrors.FailedToLoad)
        @assert(err == MyErrors.BasicErrors.FailedToLoad)
    }

    toto(E0)
    toto(E1)
}
