struct S
{
    x, y: f64 = 50
}

//#[swag.printbc]
#test
{
    var pt: S
    pt.x = 41
}