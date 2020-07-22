#[swag.printbc]
#test
{
    loop 5
    {
        switch @index
        {
            case 0: @print("0")
            case 1: @print("1")
            case 2: @print("2")
        }
    }
}