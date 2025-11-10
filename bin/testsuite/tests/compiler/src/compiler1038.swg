struct Point {}

//#[Swag.PrintBc]
#test
{
    var pt: [5] Point
    let ptr = &pt
    @init(ptr, 5)
}
