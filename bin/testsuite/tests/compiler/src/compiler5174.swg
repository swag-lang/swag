//#global testerror "Err0631"

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