#[swag.printbc]
#test
{
    loop 5
    {
        switch @index
        {
            case 0: @print("0")
        }
    }
}