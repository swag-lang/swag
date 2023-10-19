enum BasicErrors
{
    FailedToLoad
    FailedToSave
}

enum MyErrors
{
    using BasicErrors    // The enum 'BasicErrors' is nested inside
    NotFound
    NotFound1
}

#assert MyErrors.NotFound == 0
#assert MyErrors.NotFound1 == 1