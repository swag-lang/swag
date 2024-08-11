#global testerror "Err0519"

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

#[Swag.NoDuplicate]
enum MyErrors1
{
    using BasicErrors0
    using BasicErrors1
}
