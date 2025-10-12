/**
# Default Struct Packing

By default, Swag aligns struct fields similarly to the C programming language.
Each field is aligned based on the size of its type, ensuring optimal memory access.
This default behavior can be explicitly specified using '#[Swag.Pack(0)]', meaning
no additional packing adjustments are applied. Below is an example illustrating this
default alignment strategy.
*/
#test
{
    struct MyStruct
    {
        x:     bool     // offset 0: aligned to 1 byte (no padding needed)
        y:     s32      // offset 4: aligned to 4 bytes (3 bytes of padding before y)
        z:     s64      // offset 8: aligned to 8 bytes (no padding needed)
    }

    #assert(#offsetof(MyStruct.x) == 0)
    #assert(#offsetof(MyStruct.y) == 4)
    #assert(#offsetof(MyStruct.z) == 8)
    #assert(#sizeof(MyStruct) == 16)
}

/**
# Reducing Packing

Swag allows reducing the packing of struct fields using the '#[Swag.Pack]' attribute.
This attribute specifies the alignment value applied to each field, enabling more compact
struct representations. Below are examples demonstrating different levels of packing.
*/
#test
{
    #[Swag.Pack(1)]
    struct MyStruct1
    {
        x:     bool     // offset 0: 1 byte (no padding)
        y:     s32      // offset 1: 4 bytes (no padding)
    }
    #assert(#offsetof(MyStruct1.x) == 0)
    #assert(#offsetof(MyStruct1.y) == 1)
    #assert(#sizeof(MyStruct1) == 5)

    #[Swag.Pack(2)]
    struct MyStruct2
    {
        x:     bool     // offset 0: 1 byte
        y:     s32      // offset 2: 4 bytes (1 byte of padding before y)
    }
    #assert(#offsetof(MyStruct2.x) == 0)
    #assert(#offsetof(MyStruct2.y) == 2)
    #assert(#sizeof(MyStruct2) == 6)

    #[Swag.Pack(4)]
    struct MyStruct3
    {
        x:     bool     // offset 0: 1 byte
        y:     s64      // offset 4: 8 bytes (3 bytes of padding before y)
    }
    #assert(#offsetof(MyStruct3.x) == 0)
    #assert(#offsetof(MyStruct3.y) == 4)
    #assert(#sizeof(MyStruct3) == 12)
}

/**
# Struct Size and Alignment

The total size of a struct in Swag is always a multiple of the largest alignment value
among its fields. This ensures correct alignment when used within larger data structures
or arrays.
*/
#test
{
    struct MyStruct1
    {
        x:     s32      // 4 bytes
        y:     bool     // 1 byte
        // 3 bytes of padding to align to s32 size
    }

    #assert(#sizeof(MyStruct1) == 8)
}

/**
# Enforcing Alignment with 'Swag.Align'

Swag provides the '#[Swag.Align]' attribute to enforce specific alignment constraints on
an entire struct. Use it to meet hardware or performance requirements.
*/
#test
{
    struct MyStruct1
    {
        x:     bool     // 1 byte
        y:     bool     // 1 byte
    }
    #assert(#offsetof(MyStruct1.x) == 0)
    #assert(#offsetof(MyStruct1.y) == 1)
    #assert(#sizeof(MyStruct1) == 2)

    #[Swag.Align(8)]
    struct MyStruct2
    {
        x:     bool     // 1 byte
        y:     bool     // 1 byte
        // 6 bytes of padding to align struct size to 8
    }
    #assert(#offsetof(MyStruct2.x) == 0)
    #assert(#offsetof(MyStruct2.y) == 1)
    #assert(#sizeof(MyStruct2) == 8)
}

/**
# Field-Specific Alignment

Set specific alignment values for individual fields using the '#[Swag.Align]' attribute.
This provides fine-grained control over memory layout for low-level optimizations.
*/
#test
{
    struct MyStruct1
    {
        x:   bool // offset 0: 1 byte
        #[Swag.Align(8)]
        y:   bool // offset 8: aligned to 8 bytes (7 bytes of padding before y)
    }
    #assert(#sizeof(MyStruct1) == 9)

    #[Swag.Align(8)]
    struct MyStruct2
    {
        x:   bool // offset 0: 1 byte
        #[Swag.Align(4)]
        y:   bool // offset 4: aligned to 4 bytes (3 bytes of padding before y)
        // 3 bytes of padding to align struct size to 8
    }
    #assert(#sizeof(MyStruct2) == 8)
}
