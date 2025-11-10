enum BasicErrors
{
    FailedToLoad
    FailedToSave
}

enum MyErrors
{
    using BasicErrors // The enum 'BasicErrors' is nested inside
    NotFound  = 100
    NotFound1
}

#assert(MyErrors.NotFound == 100)
#assert(MyErrors.NotFound1 == 101)
