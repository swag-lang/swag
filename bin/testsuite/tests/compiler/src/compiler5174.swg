//#global testerror "Err0623"

enum BasicErrors
{
    FailedToLoad
    FailedToSave
}

enum MyErrors1
{
    using BasicErrors
}

#test
{
    var x: MyErrors1
    @assert(x == .BasicErrors.FailedToLoad)
}