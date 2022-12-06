#test
{
    // A pointer is declared with '*'
    {
        var ptr1: *u8
        var ptr2: **u8
    }

    // A pointer can be null (i know some of you will collapse here)
    {
        var ptr1: *u8
        @assert(ptr1 == null)
    }

    // You can take the address of something with '&'
    {
        arr := [1, 2, 3, 4]
        ptr := &arr[0]
        @assert(ptr[0] == 1)
        @assert(@dataof(arr) == ptr)
    }

    // Pointers can be 'const'
    {
        str := "string"
        var ptr: const *u8 = @dataof(str)
        @assert(dref ptr == "s"'u8)
    }

    // You can be weird, but is this necessary ?
    {
        var ptr:  *const *u8        // Normal pointer to a const pointer
        var ptr1: const *const *u8  // Const pointer to a const pointer
        var ptr2: const **u8        // Const pointer to a normal pointer
    }

    // If you want to enable pointer arithmetic, declare your pointer
    // with ^ instead of *
    {
        var ptr: ^u8
        ptr = ptr - 1
    }

    // Taking the address of an array element enables pointer arithmetic
    {
        var x: [4] s32
        ptr := &x[1]
        ptr = ptr - 1
    }

    // You can dereference a pointer by index if pointer arithmetic is possible, or with 'dref'
    {
        v    := 1
        ptr := &v
        @assert(@typeof(ptr) == *s32)
        @assert(dref ptr == 1)
    }

    {
        arr := [1, 2, 3, 4]
        ptr := &arr[0]
        @assert(@typeof(ptr) == ^s32)

        // The type of 'ptr' is ^s32, so it can be dereferenced by index
        value1 := ptr[0]
        @assert(value1 == 1)
        #assert @typeof(value1) == s32

        // 'deref' still works for the first element
        value := dref ptr
        @assert(value == 1)
        #assert @typeof(value) == s32
    }
}