//#[swag.printbc, swag.optim("bc", false)]
#[swag.printbc]
#test
{
    visit "toto"
    {
        @print("x")
    }
}