/**
# Single value pointers
*/
#test
{
    // A pointer to a **single element** is declared with `*`.
    {
        var ptr1: *u8   // This is a pointer to one single 'u8'
        var ptr2: **u8  // This is a pointer to one other pointer to one single 'u8'
    }

    // A pointer can be `null` (i know some of you will collapse here).
    {
        var ptr1: *u8
        @assert(ptr1 == null)
    }

    // You can take the address of something with `&`.
    {
        var arr = 1
        let ptr = &arr // Take the address of the variable 'arr'
        @assert(@typeof(ptr) == *s32)
    }

    // You can get the pointed value with `dref`.
    {
        var arr = 42
        let ptr = &arr
        @assert(dref ptr == 42)
    }

    // Pointers can be `const`.
    {
        let str = "string"
        let ptr: const *u8 = @dataof(str)
        @assert(dref ptr == "s"'u8)
    }

    // You can be weird, but is this necessary ?
    {
        var ptr:  *const *u8        // Normal pointer to a const pointer
        var ptr1: const *const *u8  // Const pointer to a const pointer
        var ptr2: const **u8        // Const pointer to a normal pointer
    }
}

/**
# Multiple values pointers
If you want to enable **pointer arithmetic**, and make a pointer to **multiple values**, declare your pointer with `^` instead of `*`.
*/

#test
{
    // `ptr` is a pointer to a memory block of `u8`.
    {
        var ptr: ^u8
        ptr = ptr - 1 // Pointer arithmetic is now possible
    }

    // Taking the address of an array element enables pointer arithmetic.
    {
        var x: [4] s32
        var ptr = &x[1]
        ptr = ptr - 1
        #assert @typeof(ptr) == ^s32
    }

    // As pointer arithmetic is enabled, you can dereference that kind of pointer by index.
    {
        var arr = [1, 2, 3, 4]
        let ptr = &arr[0]
        @assert(@typeof(ptr) == ^s32)

        // The type of 'ptr' is ^s32, so it can be dereferenced by index
        let value1 = ptr[0]
        @assert(value1 == 1)
        #assert @typeof(value1) == s32

        let value2 = ptr[1]
        @assert(value2 == 2)
        #assert @typeof(value2) == s32

        // But 'dref' still works for the first element
        let value = dref ptr
        @assert(value == 1)
        #assert @typeof(value) == s32
    }
}