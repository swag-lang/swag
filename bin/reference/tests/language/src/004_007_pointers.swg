/**
# Single Value Pointers

A pointer to a **single element** is declared using the `*` symbol. This allows you to create a pointer that holds the address of one specific instance of a data type. Pointers can be used to reference any type of data, allowing for efficient memory management and manipulation of variables.
*/
#test
{
    var ptr1: *u8           // This is a pointer to a single 'u8' value
    var ptr2: **u8          // This is a pointer to another pointer, which in turn points to a single 'u8' value
}

/**
# Null Pointers

In Swag, a pointer can be `null`, indicating that it does not point to any valid memory location. A null pointer is commonly used to signify that the pointer is uninitialized or that it intentionally points to "nothing". Checking for null pointers before dereferencing is a good practice to avoid runtime errors.
*/
#test
{
    var ptr1: *u8                  // Declaring a pointer to 'u8' without initialization
    @assert(ptr1 == null)          // By default, the pointer is null, indicating it doesn't point to any valid memory location
}

/**
# Taking the Address of a Variable

You can obtain the address of a variable using the `&` operator. This operation returns a pointer to the variable, which can then be stored and used to access or modify the variable's value indirectly through its memory address.
*/
#test
{
    var arr = 1                          // Declare an integer variable
    let ptr = &arr                        // Take the address of the variable 'arr'
    @assert(@typeof(ptr) == *s32)         // The type of 'ptr' is a pointer to 's32', the type of 'arr'
}

/**
# Dereferencing a Pointer

Dereferencing a pointer involves accessing the value stored at the memory location that the pointer refers to. In Swag, you use the `dref` intrinsic for dereferencing, which allows you to retrieve the actual data stored at the pointer's address.
*/
#test
{
    var arr = 42                          // Declare an integer variable with value 42
    let ptr = &arr                        // Take the address of 'arr'
    @assert(dref ptr == 42)               // Dereference the pointer to access the value of 'arr', which should be 42
}

/**
# Const Pointers

A `const` pointer in Swag refers to a pointer that cannot change the memory location it points to. However, the data at that memory location can still be mutable. Const pointers are useful when you want to ensure that a pointer always refers to the same memory address throughout its lifetime.
*/
#test
{
    let str  = "string"                   // Declare a string variable
    let ptr: const *u8 = @dataof(str)     // A const pointer to a 'u8' value
    @assert(dref ptr == `s`)              // Dereferencing the pointer to get the first character of the string
}

/**
# Combining `const` with Pointers

Swag allows you to combine `const` with pointers in different ways, depending on the level of immutability you need. You can create a pointer that is constant itself, a pointer to constant data, or both. The choice depends on whether you want to restrict the pointer from changing its target or the data being pointed to, or both.
*/
#test
{
    var ptr:  *const *u8              // A pointer to a const pointer to 'u8'
    var ptr1: const *const *u8        // A const pointer to a const pointer to 'u8'
    var ptr2: const **u8              // A const pointer to a normal pointer to 'u8'
}

/**
# Multiple Values Pointers

For cases where you need **pointer arithmetic** and want a pointer to **multiple values** (like an array or a memory block), you declare your pointer with `^` instead of `*`. This type of pointer is especially useful for iterating over a sequence of elements stored in contiguous memory locations.
*/
#test
{
    var ptr: ^u8                        // Declaring a pointer to a memory block of 'u8' values

    // Pointer arithmetic allows you to navigate through the memory block by modifying the pointer's address
    ptr = ptr - 1                        // Move the pointer back by one 'u8'
}

/**
# Pointer Arithmetic and Array Elements

When you take the address of an array element, the resulting pointer is treated as pointing to multiple elements, allowing for pointer arithmetic. This enables operations like incrementing or decrementing the pointer, which is useful for traversing arrays.
*/
#test
{
    var x: [4] s32                      // Declare an array of 4 's32' elements
    var ptr = &x[1]                     // Take the address of the second element

    // Pointer arithmetic is now enabled because 'ptr' is treated as pointing to multiple values
    ptr = ptr - 1                       // Move the pointer back to the first element
    #assert @typeof(ptr) == ^s32        // The type of 'ptr' is now a pointer to multiple 's32' values
}

/**
# Dereferencing with Indexes

When pointer arithmetic is enabled, you can dereference the pointer using an index, allowing access to elements in an array-like manner. This is particularly useful when working with arrays through pointers, as it allows for flexible element access.
*/
#test
{
    var arr = [1, 2, 3, 4]             // Declare and initialize an array of 's32' elements
    let ptr = &arr[0]                  // Take the address of the first element
    @assert(@typeof(ptr) == ^s32)      // The pointer type allows pointer arithmetic

    // Dereferencing by index to access the elements
    let value1 = ptr[0]                // Access the first element
    @assert(value1 == 1)               // Verify the first element's value is 1
    #assert @typeof(value1) == s32     // Assert the type of value1 is 's32'

    let value2 = ptr[1]                // Access the second element
    @assert(value2 == 2)               // Verify the second element's value is 2
    #assert @typeof(value2) == s32     // Assert the type of value2 is 's32'

    // Using 'dref' for the first element, equivalent to ptr[0]
    let value = dref ptr               // Dereference the pointer to access the first element
    @assert(value == 1)                // Verify the dereferenced value is 1
    #assert @typeof(value) == s32      // Assert the type of the dereferenced value is 's32'
}
