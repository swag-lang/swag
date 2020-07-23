#[swag.printbc]
#test
{
    x := 10'f64
    if(x == 1 || x == 78)
        @print("true")
    else
        @print("false")
}