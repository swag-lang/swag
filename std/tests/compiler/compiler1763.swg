struct A
{

}

impl A
{
    func opCmp(using self, other: A)->s32
    {
        return 0
    }
}

//#[swag.printbc]
#test
{
    var x, y: A
    if x >= y
        @print("true")
    else
        @print("false")
}