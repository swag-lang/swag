#test
{
    // The default struct packing is the same as in C (each field is aligned to the size of the type).
    {
        struct MyStruct
        {
            x: bool     // 1 byte

                        // 3 bytes of padding
            y: s32      // 4 bytes

            z: s64      // 8 bytes
        }

        #assert @offsetof(MyStruct.x) == 0
        #assert @offsetof(MyStruct.y) == 4
        #assert @offsetof(MyStruct.z) == 8
        #assert @sizeof(MyStruct)     == 16
    }

    // You can reduce the packing of the fields with #[Swag.Pack].
    {
        #[Swag.Pack(1)]
        struct MyStruct1
        {
            x: bool // 1 byte
            y: s32  // 4 bytes
        }

        #assert @offsetof(MyStruct1.x) == 0
        #assert @offsetof(MyStruct1.y) == 1
        #assert @sizeof(MyStruct1)     == 5

        #[Swag.Pack(2)]
        struct MyStruct2
        {
            x: bool // 1 byte

                    // 1 byte of padding
            y: s32  // 4 bytes
        }

        #assert @offsetof(MyStruct2.x) == 0
        #assert @offsetof(MyStruct2.y) == 2
        #assert @sizeof(MyStruct2)     == 6

        #[Swag.Pack(4)]
        struct MyStruct3
        {
            x: bool // 1 byte

                    // 3 bytes of padding
            y: s32  // 4 bytes
        }

        #assert @offsetof(MyStruct3.x) == 0
        #assert @offsetof(MyStruct3.y) == 4
        #assert @sizeof(MyStruct3)     == 8
    }

    // You can specify the struct alignement with #[Swag.Align].
    {
        #[Swag.Pack(8)]
        struct MyStruct1
        {
            x: bool     // 1 byte
            y: bool     // 1 byte
        }

        #assert @offsetof(MyStruct1.x) == 0
        #assert @offsetof(MyStruct1.y) == 1
        #assert @sizeof(MyStruct1)     == 2

        #[Swag.Pack(8), Swag.Align(8)]
        struct MyStruct2
        {
            x: bool     // 1 byte
            y: bool     // 1 byte
                        // 6 bytes of padding
        }

        #assert @offsetof(MyStruct2.x) == 0
        #assert @offsetof(MyStruct2.y) == 1
        #assert @sizeof(MyStruct2)     == 8
    }
}
