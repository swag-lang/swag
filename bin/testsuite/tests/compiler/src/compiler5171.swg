//#global testerror "Err0519"

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
    using BasicErrors
}

enum MyErrors
{
    using MyErrors0
    using MyErrors1
}

#test
{
    let x = BasicErrors.FailedToLoad
    let y: MyErrors0 = x
    let z: MyErrors1 = x
}