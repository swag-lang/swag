struct MyStruct
{
    x:     s32 = 10
    y:     s32 = 20
    z:     s32 = 30
}

/**
`opVisit` is a versatile macro used for iteration over elements in a struct, but it is not limited to fields. It can be used to visit any kind of data the struct holds, such as elements of a dynamic array, internal buffers, or complex object graphs.

The `#[Swag.Macro]` attribute is mandatory for `opVisit`.
It is also a generic function with two compile-time boolean parameters:
* `ptr`: If `true`, visit elements by pointer (address).
* `back`: If `true`, visit elements in reverse order (last to first).
*/
impl MyStruct
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(self, stmt: code)
    {
        // `ptr` and `back` provide flexibility for visiting by reference or in reverse order.
        #if ptr:
            #error "visiting by pointer is not supported in this example"

        #if back:
            #error "reverse visiting is not supported in this example"

        // Example of visiting the fields of `MyStruct`. This is just one way to use `opVisit`.
        loop idx in 3
        {
            // `#macro` ensures the code is injected into the caller's scope.
            #macro
            {
                // `#alias0` is used for the current value being visited.
                var #alias0: s32 = undefined

                // Access the correct field based on the current `idx`.
                switch #up idx
                {
                case 0:
                    #alias0 = #up self.x
                case 1:
                    #alias0 = #up self.y
                case 2:
                    #alias0 = #up self.z
                }

                // `#alias1` holds the current index of the iteration.
                var #alias1 = #index

                // Insert the user-defined logic from the calling scope.
                #mixin #up stmt
            }
        }
    }
}

/**
This is just one way to use `opVisit`, visiting the struct's fields. You can apply the same concept to visit more complex data, like elements in a dynamic array or other internal structures.
*/

#test
{
    var myStruct = MyStruct{}
    var cpt = 0

    // Visit each field of `MyStruct` in declaration order.
    // `v` corresponds to the value (i.e., #alias0).
    // `i` corresponds to the index (i.e., #alias1).
    visit v, i in myStruct
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
You can also implement different versions of `opVisit` to handle other data. For example, if your struct contains dynamic arrays or buffers, you can iterate over those elements using the same macro structure.

Here, we'll demonstrate a variant that visits the fields in reverse order.
*/
impl MyStruct
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisitReverse(stmt: code)
    {
        // In this version, we visit the fields in reverse order.
        loop idx in 3
        {
            #macro
            {
                var #alias0: s32 = undefined
                switch #up idx
                {
                case 0:
                    #alias0 = #up self.z
                case 1:
                    #alias0 = #up self.y
                case 2:
                    #alias0 = #up self.x
                }

                var #alias1 = #index
                #mixin #up stmt
            }
        }
    }
}

/**
The `opVisitReverse` variant allows us to visit the struct in reverse order.
*/
#test
{
    var myStruct = MyStruct{}
    var cpt = 0

    // Call the variant `opVisitReverse` to iterate over the fields in reverse order.
    visit(Reverse) v, i in myStruct
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
Beyond struct fields, `opVisit` can be designed to visit elements in dynamic arrays, buffers, or other types of data. The flexibility of `opVisit` means it can adapt to whatever data structure the struct holds.

For instance, if you had a struct with a slice:
*/
struct SliceStruct
{
    buffer: [] s32 = [1, 2, 3, 4, 5]
}

/**
You could define an `opVisit` that iterates over the elements of the `buffer` rather than the struct's fields.
*/
impl SliceStruct
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(self, stmt: code)
    {
        loop idx in @countof(self.buffer)
        {
            #macro
            {
                // #alias0 represents the value of the current buffer element.
                var #alias0 = #up self.buffer[#up idx]

                // #alias1 represents the current index.
                var #alias1 = #index

                // Insert the user-provided logic from the caller.
                #mixin #up stmt
            }
        }
    }
}

#test
{
    var arrStruct = SliceStruct{}
    var sum = 0

    // Visit each element in the dynamic array buffer.
    visit v, i in arrStruct
    {
        sum += v
    }

    @assert(sum == 1 + 2 + 3 + 4 + 5)
}
