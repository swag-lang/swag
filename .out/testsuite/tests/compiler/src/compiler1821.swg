//#[Swag.printbc]
#test
{
    a := @[1,2,3,4]
    s := a[0..3]
    @assert(&a[0] == &s[0])
}