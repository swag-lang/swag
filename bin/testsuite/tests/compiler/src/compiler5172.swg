//#global testerror "Err0005"

enum BasicErrors
{
    FailedToLoad
    FailedToSave
}

enum MyErrors0
{
    using BasicErrors
}

enum MyErrors1
{
    using MyErrors0
}

#test
{
    let x = BasicErrors.FailedToLoad
    let y: MyErrors1 = x
}