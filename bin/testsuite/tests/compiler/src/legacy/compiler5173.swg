//#global testerror "Err0004"

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
