//#[swag.printbc, swag.optim("bc", false)]
#[swag.printbc]
#test
{
    loop "toto"
    {
        @print("x")
    }
}