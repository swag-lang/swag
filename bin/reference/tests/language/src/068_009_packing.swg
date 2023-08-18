/**
You can also control the struct layout with two attributes: `#[Swag.Pack]` and `#[Swag.Align]`.
*/

#test
{
    // The default struct packing is the same as in C: each field is aligned to the size of the type. This is the equivalent of `#[Swag.Pack(0)]`.
    {
        struct MyStruct
        {
            x: bool     // offset 0: aligned to 1 byte
            y: s32      // offset 4: s32 is aligned to 4 bytes (so here there's 3 bytes of padding before)
            z: s64      // offset 8: aligned to 8 bytes
        }

        // '@offsetof' can be used to retrieve the offset of a field
        #assert @offsetof(MyStruct.x) == 0
        #assert @offsetof(MyStruct.y) == 4
        #assert @offsetof(MyStruct.z) == 8
        #assert @sizeof(MyStruct)     == 16
    }

    // You can *reduce* the packing of the fields with `#[Swag.Pack]`.
    {
        #[Swag.Pack(1)]
        struct MyStruct1
        {
            x: bool // offset 0: 1 byte
            y: s32  // offset 1: 4 bytes (no padding)
        }

        #assert @offsetof(MyStruct1.x) == 0
        #assert @offsetof(MyStruct1.y) == 1
        #assert @sizeof(MyStruct1)     == 5

        #[Swag.Pack(2)]
        struct MyStruct2
        {
            x: bool // offset 0: 1 byte
            y: s32  // offset 2: 4 bytes (1 byte of padding before)
        }

        #assert @offsetof(MyStruct2.x) == 0
        #assert @offsetof(MyStruct2.y) == 2
        #assert @sizeof(MyStruct2)     == 6

        #[Swag.Pack(4)]
        struct MyStruct3
        {
            x: bool // offset 0: 1 byte
            y: s64  // offset 4: 8 bytes (3 bytes of padding before)
        }

        #assert @offsetof(MyStruct3.x) == 0
        #assert @offsetof(MyStruct3.y) == 4
        #assert @sizeof(MyStruct3)     == 12
    }

    // The total struct size is always a multiple of the biggest alignement of the fields.
    {
        struct MyStruct1
        {
            x: s32  // 4 bytes
            y: bool // 1 byte
            // 3 bytes of padding because of 'x', to aligned on 's32'
        }

        #assert @sizeof(MyStruct1) == 8
    }

    // You can force the struct alignement with `#[Swag.Align]`.
    {
        struct MyStruct1
        {
            x: bool     // 1 byte
            y: bool     // 1 byte
        }

        #assert @offsetof(MyStruct1.x) == 0
        #assert @offsetof(MyStruct1.y) == 1
        #assert @sizeof(MyStruct1)     == 2

        #[Swag.Align(8)]
        struct MyStruct2
        {
            x: bool     // 1 byte
            y: bool     // 1 byte
            // 6 bytes of padding to be a multiple of '8'
        }

        #assert @offsetof(MyStruct2.x) == 0
        #assert @offsetof(MyStruct2.y) == 1
        #assert @sizeof(MyStruct2)     == 8
    }

    // You can also force each field to be aligned on a specific value.
    {
        struct MyStruct1
        {
            x: bool // offset 0: 1 byte
            #[Swag.Align(8)]
            y: bool // offset 8: aligned to 8 (7 bytes of padding before)
        }

        #assert @sizeof(MyStruct1) == 9

        #[Swag.Align(8)]
        struct MyStruct2
        {
            x: bool // offset 0: 1 byte
            #[Swag.Align(4)]
            y: bool // offset 4: aligned to 4 (3 bytes of padding before)
            // 3 bytes of padding to be a multiple of 8
        }

        #assert @sizeof(MyStruct2) == 8
    }
}
