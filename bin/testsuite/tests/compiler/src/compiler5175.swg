#global testerror "Err0684"

enum BasicErrors0
{
    FailedToLoad
    FailedToSave
}

enum BasicErrors1
{
    FailedToLoad
    FailedToSave
}

enum MyErrors1
{
    using BasicErrors0
    using BasicErrors1
}
