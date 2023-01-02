#global testerror
#test
{
    a := 0'u8

    #[Swag.Complete]
    switch a
    {
    case 1:     break
    default:    break
    }
}
