struct A {}
impl A
{
    mtd opDrop()
    {
        //@print("drop")
    }
}

typealias B = A
var x:  [4] B
var x1: B
