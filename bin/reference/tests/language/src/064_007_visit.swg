struct MyStruct
{
    x: s32 = 10
    y: s32 = 20
    z: s32 = 30
}

/**
You can visit a struct variable if a macro `opVisit` has been defined. This is the equivalent of an **iterator**.
*/

/**
`opVisit` is a macro, so it should be marked with the `#[Swag.Macro]` attribute.
`opVisit` is also a generic function which takes two compile time generic parameters of type `bool`.
* if 'ptr' is 'true', indicates that we should visit by address
* if 'back' is 'true", indicated that we want to visit in reverse order (from back to front)
*/
impl MyStruct
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(self, stmt: code)
    {
        // 'ptr' is a generic parameter that tells if we want to visit by pointer or by value.
        // We do not use it in this example, so we check at compile time that it's not true.
        #if ptr #error "visiting myStruct by pointer is not supported"

        // 'back' is a generic parameter that tells if we want to visit in reverse order.
        // We do not use it in this example, so we check at compile time that it's not true.
        #if ptr #error "visiting myStruct in reverse order is not supported"

        // Loop on the 3 fields
        loop idx: 3
        {
            // The '#macro' keyword forces its body to be in the scope of the caller
            #macro
            {
                // @alias0 will be the value
                var @alias0: s32 = undefined

                // As this code is in the caller scope, with need to add a '#up' before 'idx' to
                // reference the variable of this function (and not a potential variable in
                // the caller scope)
                switch #up idx
                {
                case 0: @alias0 = #up self.x   // Same for function parameter 'self'
                case 1: @alias0 = #up self.y
                case 2: @alias0 = #up self.z
                }

                // @alias1 will be the index
                var @alias1 = @index

                // include user code
                #mixin #up stmt
            }
        }
    }
}

/**
So now that the `opVisit` has been defined, we can `visit` it.
*/
#test
{
    var myStruct = MyStruct{}
    var cpt = 0

    // Visiting each field in declaration order
    // 'v' is an alias for @alias0 (value)
    // 'i' is an alias for @alias1 (index)
    visit v, i: myStruct
    {
        switch i
        {
        case 0: @assert(v == 10)
        case 1: @assert(v == 20)
        case 2: @assert(v == 30)
        }

        cpt += 1
    }

    @assert(cpt == 3)
}

/**
You can have variants of `opVisit` by specifying an **additional name** after a comma.
*/
impl MyStruct
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit,reverse(stmt: code)   // We add a variant called 'reverse'
    {
        // Visit fields in reverse order (z, y then x).
        // Note that visiting in reverse order should be implemented in the normal 'opVisit'
        // function by checking the 'back' generic parameter. But this is just to make the
        // point of special dedicated visit versions.
        loop idx: 3
        {
            #macro
            {
                var @alias0: s32 = undefined
                switch #up idx
                {
                case 0: @alias0 = #up self.z
                case 1: @alias0 = #up self.y
                case 2: @alias0 = #up self.x
                }

                var @alias1 = @index
                #mixin #up stmt
            }
        }
    }
}

#test
{
    var myStruct = MyStruct{}
    var cpt = 0

    // To call a variant, add the extra name after a comma.
    visit,reverse v, i: myStruct
    {
        switch i
        {
        case 0: @assert(v == 30)
        case 1: @assert(v == 20)
        case 2: @assert(v == 10)
        }

        cpt += 1
    }

    @assert(cpt == 3)
}