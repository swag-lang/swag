#global testerror "Err0119"

enum E : f32
{
    FailedToLoad  = 1.1
    FailedToLoad1 = 1.2
}

#test
{
    let x = E.FailedToLoad
    #[Swag.Complete]
    switch x
    {
    case FailedToLoad:
        break
    }
}
