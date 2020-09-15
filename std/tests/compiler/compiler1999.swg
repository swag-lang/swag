struct A
{
    x: s32
}

impl A
{
    func toto()
    {
        @print("toto\n")
    }
}

#test
{
    ptr := @typeof(A)
    //f := cast(func()) ptr.methods[0]
    visit it: ptr.methods
    {
        @print(it.name)
    }
}