/**
You can control the layout of struct fields with the attributes `#[Swag.Pack]` and `#[Swag.Align]`. These attributes allow you to specify how tightly packed or aligned your data should be in memory.
*/

/**
The default struct packing in Swag is similar to C, where each field is aligned based on the size of its type. This is equivalent to `#[Swag.Pack(0)]`, meaning no additional packing is applied.
*/
#test
{
    struct MyStruct
    {
        x:     bool     // offset 0: aligned to 1 byte (no padding needed)
        y:     s32      // offset 4: aligned to 4 bytes (3 bytes of padding before y)
        z:     s64      // offset 8: aligned to 8 bytes (no padding needed)
    }

    #assert @offsetof(MyStruct.x) == 0
    #assert @offsetof(MyStruct.y) == 4
    #assert @offsetof(MyStruct.z) == 8
    #assert @sizeof(MyStruct) == 16
}

/**
You can reduce the packing of fields using `#[Swag.Pack]`, which determines the alignment of each field to the specified value.
*/
#test
{
    #[Swag.Pack(1)]
    struct MyStruct1
    {
        x:     bool     // offset 0: 1 byte (no padding)
        y:     s32      // offset 1: 4 bytes (no padding)
    }
    #assert @offsetof(MyStruct1.x) == 0
    #assert @offsetof(MyStruct1.y) == 1
    #assert @sizeof(MyStruct1) == 5

    #[Swag.Pack(2)]
    struct MyStruct2
    {
        x:     bool     // offset 0: 1 byte
        y:     s32      // offset 2: 4 bytes (1 byte of padding before y)
    }
    #assert @offsetof(MyStruct2.x) == 0
    #assert @offsetof(MyStruct2.y) == 2
    #assert @sizeof(MyStruct2) == 6

    #[Swag.Pack(4)]
    struct MyStruct3
    {
        x:     bool     // offset 0: 1 byte
        y:     s64      // offset 4: 8 bytes (3 bytes of padding before y)
    }
    #assert @offsetof(MyStruct3.x) == 0
    #assert @offsetof(MyStruct3.y) == 4
    #assert @sizeof(MyStruct3) == 12
}

/**
The total struct size is always a multiple of the largest alignment of its fields.
*/
#test
{
    struct MyStruct1
    {
        x:     s32      // 4 bytes
        y:     bool     // 1 byte
        // 3 bytes of padding to align with s32 size
    }
    #assert @sizeof(MyStruct1) == 8
}

/**
You can enforce struct alignment with `#[Swag.Align]`.
*/
#test
{
    struct MyStruct1
    {
        x:     bool     // 1 byte
        y:     bool     // 1 byte
    }
    #assert @offsetof(MyStruct1.x) == 0
    #assert @offsetof(MyStruct1.y) == 1
    #assert @sizeof(MyStruct1) == 2

    #[Swag.Align(8)]
    struct MyStruct2
    {
        x:     bool     // 1 byte
        y:     bool     // 1 byte
        // 6 bytes of padding to align struct size to 8
    }
    #assert @offsetof(MyStruct2.x) == 0
    #assert @offsetof(MyStruct2.y) == 1
    #assert @sizeof(MyStruct2) == 8
}

/**
You can also set alignment for individual fields.
*/
#test
{
    struct MyStruct1
    {
        x: bool // offset 0: 1 byte
        #[Swag.Align(8)]
        y: bool // offset 8: aligned to 8 bytes (7 bytes of padding before y)
    }
    #assert @sizeof(MyStruct1) == 9

    #[Swag.Align(8)]
    struct MyStruct2
    {
        x: bool // offset 0: 1 byte
        #[Swag.Align(4)]
        y: bool // offset 4: aligned to 4 bytes (3 bytes of padding before y)
        // 3 bytes of padding to align struct size to 8
    }
    #assert @sizeof(MyStruct2) == 8
}
