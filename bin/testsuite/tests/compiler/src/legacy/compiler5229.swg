struct A {}
impl A
{
    mtd opDrop()
    {
        //@print("drop")
    }
}

alias B = A
var x:  [4] B
var x1: B
