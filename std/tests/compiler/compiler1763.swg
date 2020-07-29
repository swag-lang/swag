#[swag.printbc]
#test
{
    x := 10
    switch x
    {
    case 0, 2:
        @print("0")
    case 1:
        @print("1")
    case 10:
        @print("10")
    default:
        @print("20")
    }
}