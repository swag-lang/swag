#global testerror "Err0326"
#test
{
    var a = 0'u8

    #[Swag.Complete]
    switch a
    {
    case 1:
        break
    default:
        break
    }
}
