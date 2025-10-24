#global testerror "Err0489"

enum BasicErrors0
{
    FailedToLoad
}

enum MyErrors1
{
    using BasicErrors0
    FailedToSave = 1
}

#test
{
    var x: MyErrors1

    #[Swag.Complete]
    switch x
    {
    case BasicErrors0.FailedToLoad:
        break
    }
}
