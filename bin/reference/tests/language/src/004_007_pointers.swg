/**
# Single-Value Pointers

A pointer to a **single element** is declared using the `*` symbol.
It represents the memory address of one instance of a given type.

Pointers can reference any type of data and are fundamental for efficient memory access and manipulation.
*/
#test
{
    var ptr1: *u8      // Pointer to a single 'u8' value
    var ptr2: **u8     // Pointer to another pointer to 'u8'
}

/**
# Null Pointers

In Swag, a pointer can be `null`, meaning it does not refer to any valid memory location.
Null pointers are often used to indicate uninitialized or intentionally empty references.

It is good practice to check for null pointers before dereferencing them to avoid runtime errors.
*/
#test
{
    var ptr1: *u8
    @assert(ptr1 == null) // Uninitialized pointers are null by default
}

/**
# Taking the Address of a Variable

The `&` operator returns the **address** of a variable, producing a pointer to it.
This allows you to access or modify a variable indirectly via its memory address.
*/
#test
{
    var value = 1
    let ptr   = &value
    @assert(#typeof(ptr) == *s32) // The pointer type matches the variable's type
}

/**
# Dereferencing a Pointer

To access the value stored at a pointer’s address, use the `dref` intrinsic.
Dereferencing retrieves the data located at the memory address referenced by the pointer.
*/
#test
{
    var value = 42
    let ptr   = &value
    @assert(dref ptr == 42) // Access the value stored at the pointer
}

/**
# Const Pointers

A `const` pointer is one whose **target address cannot change** once assigned.
However, the data at that address may still be modified (unless declared const itself).
Const pointers help guarantee that a pointer always refers to the same memory location.
*/
#test
{
    let str  = "string"
    let ptr: const *u8 = @dataof(str)
    @assert(dref ptr == 's') // Access the first byte of the string
}

/**
# Combining `const` with Pointers

Swag allows fine-grained control over constness in pointer declarations.
You can define:
- a **pointer to const data**,
- a **const pointer**, or
- a **const pointer to const data**.

This provides strong control over both pointer immutability and data mutability.
*/
#test
{
    var ptr:  *const *u8           // Pointer to a const pointer
    var ptr1: const *const *u8     // Const pointer to a const pointer
    var ptr2: const **u8           // Const pointer to a mutable pointer
}

/**
# Multi-Value Pointers

To enable **pointer arithmetic** or reference **contiguous memory blocks**,
use `[*]` instead of `*`.

This type of pointer behaves like a view over multiple elements,
allowing indexed access and pointer movement through memory.
*/
#[Swag.Sanity(false)]
#test
{
    var ptr: [*] u8 // Pointer to a block of 'u8' values
    ptr = ptr - 1 // Move the pointer back by one element
}

/**
# Pointer Arithmetic and Array Elements

When taking the address of an array element, the resulting pointer automatically
supports pointer arithmetic since it refers to a sequence of elements.
*/
#test
{
    var arr: [4] s32
    var ptr  = &arr[1]

    // The pointer is treated as a multi-value pointer
    ptr = ptr - 1
    #assert(#typeof(ptr) == [*] s32)
}

/**
# Dereferencing with Indexing

When pointer arithmetic is available, you can dereference a pointer using indexes,
just like accessing elements in an array.

This is particularly useful when traversing arrays or buffers via pointers.
*/
#test
{
    var arr = [1, 2, 3, 4]
    let ptr = &arr[0]
    @assert(#typeof(ptr) == [*] s32)

    let value1 = ptr[0]
    @assert(value1 == 1)
    #assert(#typeof(value1) == s32)

    let value2 = ptr[1]
    @assert(value2 == 2)
    #assert(#typeof(value2) == s32)

    let value = dref ptr
    @assert(value == 1)
    #assert(#typeof(value) == s32)
}
