func toto(x: [..] f64)
{
    @print(x[0])
}

#[swag.printbc]
#test
{
     pt := [10'f64, 20, 30]
     toto(pt)
}