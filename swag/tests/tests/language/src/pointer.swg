#test
{
    // A pointer is declared with '*'
    {
        var ptr1: *u8
        var ptr2: **u8 = null
    }

    // You can take the address of something with '&'
    {
        arr := @[1, 2, 3, 4]
        ptr := &arr[0]
        @assert(ptr[0] == 1)
    }

    // You can dereference a pointer by index, or with ':ptr'
    {
        arr := @[1, 2, 3, 4]
        ptr := &arr[0]

        value := :ptr
        @assert(value == 1)
        value1 := ptr[0]
        @assert(value1 == 1)
    }

    // Pointers can be 'const'
    {
        str := "string"
        var ptr: const *u8 = @dataof(str)
        @assert(ptr[0] == "s"'u8)
    }
}