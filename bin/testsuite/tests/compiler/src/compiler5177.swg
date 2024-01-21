#global testerror "Err0067"

enum BasicErrors0
{
    FailedToLoad
}

#[Swag.NoDuplicate]
enum MyErrors1
{
    using BasicErrors0
    FailedToSave = 0
}
