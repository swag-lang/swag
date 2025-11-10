struct MyStruct
{
    x, y: bool
    #[Swag.Align(8)]
    z:   bool
}

#assert(#offsetof(MyStruct.z) == 8)
#assert(#sizeof(MyStruct) == 9)
