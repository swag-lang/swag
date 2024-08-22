/**
# Single Value Pointers

A pointer to a **single element** is declared with `*`. This allows you to create a pointer that can hold the address of one specific instance of a data type.
*/
#test
{
    var ptr1: *u8      // This is a pointer to a single 'u8' value
    var ptr2: **u8     // This is a pointer to another pointer, which in turn points to a single 'u8' value
}

/**
# Null Pointers

A pointer can be `null`, meaning it does not point to any valid memory location. In Swag, like in many other languages, a null pointer is often used to indicate that the pointer is not yet initialized or intentionally points to "nothing".
*/
#test
{
    var ptr1: *u8  // Declaring a pointer to 'u8' without initialization
    @assert(ptr1 == null)  // By default, the pointer is null, indicating it doesn't point to any valid memory location
}

/**
# Taking the Address of a Variable

You can take the address of a variable using the `&` operator. This operator allows you to obtain the memory address of a variable, which can then be stored in a pointer.
*/
#test
{
    var arr = 1
    let ptr = &arr  // Take the address of the variable 'arr'
    @assert(@typeof(ptr) == *s32)  // The type of 'ptr' is a pointer to 's32'
}

/**
# Dereferencing a Pointer

You can get the value that a pointer is pointing to by using the `dref` intrinsic. Dereferencing a pointer means accessing the data stored at the memory location that the pointer holds.
*/
#test
{
    var arr = 42
    let ptr = &arr  // Take the address of 'arr'
    @assert(dref ptr == 42)  // Dereference the pointer to access the value of 'arr'
}

/**
# Const Pointers

Pointers can be `const`, meaning that the pointer itself cannot change what it points to, though the data being pointed to may still be mutable. This is useful when you want to ensure that a pointer always refers to the same memory location.
*/
#test
{
    let str  = "string"
    let ptr: const *u8 = @dataof(str)  // A const pointer to a 'u8' value
    @assert(dref ptr == `s`)  // Dereferencing the pointer to get the first character of the string
}

/**
# Combining `const` with Pointers

You can be more specific with pointers by combining `const` in different ways, but consider if it's necessary for your use case. For example, you can have pointers that are themselves constant, pointers to constant data, or both.
*/
#test
{
    var ptr:  *const *u8           // A normal pointer to a const pointer to 'u8'
    var ptr1: const *const *u8     // A const pointer to a const pointer to 'u8'
    var ptr2: const **u8           // A const pointer to a normal pointer to 'u8'
}

/**
# Multiple Values Pointers

If you want to enable **pointer arithmetic** and make a pointer to **multiple values** (essentially treating the pointer as pointing to an array or a memory block), declare your pointer with `^` instead of `*`. This is useful when you need to iterate over a sequence of elements in memory.
*/
#test
{
    var ptr: ^u8  // Declaring a pointer to a memory block of 'u8' values

    // Pointer arithmetic allows you to move the pointer to different elements in the memory block
    ptr = ptr - 1  // Move the pointer back by one 'u8'
}

/**
# Pointer Arithmetic and Array Elements

Taking the address of an array element allows for pointer arithmetic. When you take the address of an element in an array, the resulting pointer is treated as pointing to multiple elements, enabling arithmetic operations like incrementing or decrementing the pointer.
*/
#test
{
    var x: [4] s32  // Declare an array of 4 s32 elements
    var ptr = &x[1]  // Take the address of the second element

    // Pointer arithmetic is now enabled because 'ptr' is treated as pointing to multiple values
    ptr = ptr - 1  // Move the pointer back to the first element
    #assert @typeof(ptr) == ^s32  // The type of ptr is now a pointer to multiple 's32' values
}

/**
# Dereferencing with Indexes

As pointer arithmetic is enabled, you can dereference that kind of pointer by using an index. This allows you to access elements in an array-like manner, even though you're working with a pointer.
*/
#test
{
    var arr = [1, 2, 3, 4]  // Declare and initialize an array of s32 elements
    let ptr = &arr[0]  // Take the address of the first element
    @assert(@typeof(ptr) == ^s32)  // The pointer type allows pointer arithmetic

    // Dereferencing by index to access the elements
    let value1 = ptr[0]  // Access the first element
    @assert(value1 == 1)
    #assert @typeof(value1) == s32

    let value2 = ptr[1]  // Access the second element
    @assert(value2 == 2)
    #assert @typeof(value2) == s32

    // You can still use 'dref' for the first element, which is equivalent to ptr[0]
    let value = dref ptr  // Dereference the pointer to access the first element
    @assert(value == 1)
    #assert @typeof(value) == s32
}
