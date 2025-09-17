struct MyStruct
{
    x:     s32 = 10  // Field 'x' initialized with default value 10
    y:     s32 = 20  // Field 'y' initialized with default value 20
    z:     s32 = 30  // Field 'z' initialized with default value 30
}

/**
# Introduction to `opVisit`

`opVisit` is a highly flexible macro used for iterating over the elements in a struct.
This macro is not confined to just the struct's fields; it can also be used to traverse
any data the struct contains, such as elements in dynamic arrays, internal buffers, or
complex object graphs.

The `#[Swag.Macro]` attribute is mandatory when using `opVisit`. It defines the macro's
behavior, allowing it to be integrated seamlessly into the codebase.

`opVisit` is a generic function that accepts two compile-time boolean parameters:

- `ptr`: When set to `true`, elements are visited by pointer (address), enabling reference-based operations.
- `back`: When set to `true`, elements are visited in reverse order (from last to first).
*/
impl MyStruct
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(me, stmt: #code void)
    {
        // The `ptr` and `back` parameters offer flexibility, allowing for reference-based
        // or reverse-order iterations.

        #if ptr:
            #error("Visiting by pointer is not supported in this example.")

        #if back:
            #error("Reverse visiting is not supported in this example.")

        // Example of visiting the fields of `MyStruct`. This demonstrates a common use case of `opVisit`.
        for idx in 3
        {
            // The `#macro` directive ensures that the code is injected into the caller's scope.
            #macro
            {
                // `#alias0` is used to represent the current value being visited.
                var #alias0: s32 = undefined

                // Access the appropriate field based on the current `idx`.
                switch #up idx
                {
                case 0:
                    #alias0 = #up me.x  // Accessing field 'x'
                case 1:
                    #alias0 = #up me.y  // Accessing field 'y'
                case 2:
                    #alias0 = #up me.z  // Accessing field 'z'
                }

                // `#alias1` holds the current index of the iteration.
                var #alias1 = @index

                // Insert user-defined logic from the calling scope.
                #inject(#up stmt)
            }
        }
    }
}

/**
# Iterating Over Struct Fields

This example demonstrates one way to use `opVisit` for iterating over the fields of a struct.
The same approach can be extended to foreach more complex data structures.
*/
#test
{
    var myStruct = MyStruct{}
    var cpt = 0

    // Visit each field of `MyStruct` in declaration order.
    // `v` corresponds to the value (i.e., #alias0).
    // `i` corresponds to the index (i.e., #alias1).
    foreach v, i in myStruct
    {
        switch i
        {
        case 0:
            @assert(v == 10)
        case 1:
            @assert(v == 20)
        case 2:
            @assert(v == 30)
        }

        cpt += 1
    }

    @assert(cpt == 3)
}

/**
# Extending `opVisit`: Reverse Order Iteration

You can also implement different versions of `opVisit` to handle other data structures.
For instance, you may want to foreach the fields in reverse order.
*/
impl MyStruct
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisitReverse(stmt: #code void)
    {
        // In this version, we foreach the fields in reverse order.
        for idx in 3
        {
            #macro
            {
                var #alias0: s32 = undefined
                switch #up idx
                {
                case 0:
                    #alias0 = #up me.z  // Accessing field 'z' in reverse order
                case 1:
                    #alias0 = #up me.y  // Accessing field 'y' in reverse order
                case 2:
                    #alias0 = #up me.x  // Accessing field 'x' in reverse order
                }

                var #alias1 = @index
                #inject(#up stmt)
            }
        }
    }
}

/**
# Reverse Order Iteration

The `opVisitReverse` variant allows us to foreach the struct's fields in reverse order,
providing flexibility depending on the needs of your application.
*/
#test
{
    var myStruct = MyStruct{}
    var cpt = 0

    // Call the variant `opVisitReverse` to iterate over the fields in reverse order.
    foreach #Reverse v, i in myStruct
    {
        switch i
        {
        case 0:
            @assert(v == 30)
        case 1:
            @assert(v == 20)
        case 2:
            @assert(v == 10)
        }

        cpt += 1
    }

    @assert(cpt == 3)
}

/**
# Visiting Elements in Dynamic Arrays

Beyond struct fields, `opVisit` can be designed to foreach elements in dynamic arrays, buffers,
or other types of data. The flexibility of `opVisit` means it can adapt to whatever data
structure the struct holds.

For example, consider a struct with a slice:
*/
struct SliceStruct
{
    buffer: [?] s32 = [1, 2, 3, 4, 5]  // A dynamic array (slice) initialized with values
}

/**
# Custom `opVisit` for Dynamic Arrays

You could define an `opVisit` that iterates over the elements of the `buffer` rather than
the struct's fields.
*/
impl SliceStruct
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(me, stmt: #code void)
    {
        for idx in @countof(me.buffer)
        {
            #macro
            {
                // #alias0 represents the value of the current buffer element.
                var #alias0 = #up me.buffer[#up idx]

                // #alias1 represents the current index.
                var #alias1 = @index

                // Insert the user-provided logic from the caller.
                #inject(#up stmt)
            }
        }
    }
}

/**
# Iterating Over a Dynamic Array

This example shows how to foreach each element in a dynamic array (slice) and perform
operations such as summing the elements.
*/
#test
{
    var arrStruct = SliceStruct{}
    var sum = 0

    // Visit each element in the dynamic array buffer.
    foreach v, i in arrStruct
    {
        sum += v
    }

    @assert(sum == 1 + 2 + 3 + 4 + 5)  // Ensuring the sum of the elements is correct
}
