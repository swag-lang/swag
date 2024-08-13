#global testerror "Err0520"

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
