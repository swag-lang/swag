//#global testerror "Err0626"

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
    x = .BasicErrors.FailedToLoad
}
