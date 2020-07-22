#[swag.printbc]
#test
{
    x := 1'f64
    if x < 2
        @print("true")
    else
        @print("false")
}