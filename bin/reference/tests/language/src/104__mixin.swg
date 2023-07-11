#test
{
    // A mixin is declared like a function, with the attribute `#[Swag.Mixin]`.
    {
        #[Swag.Mixin]
        func myMixin()
        {
        }
    }

    // A mixin function is inserted in the scope of the caller.
    {
        #[Swag.Mixin]
        func myMixin()
        {
            a += 1
        }

        var a = 0
        myMixin()   // Equivalent to 'a += 1'
        myMixin()   // Equivalent to 'a += 1'
        @assert(a == 2)
    }

    // This behaves like a function, so you can add parameters.
    {
        #[Swag.Mixin]
        func myMixin(increment: s32 = 1)
        {
            a += increment
        }

        var a = 0
        myMixin()   // Equivalent to 'a += 1'
        myMixin(2)  // Equivalent to 'a += 2'
        @assert(a == 3)
    }

    // A mixin accepts parameters of type `code`.
    {
        #[Swag.Mixin]
        func doItTwice(what: code)
        {
            // You can then insert the code with '#mixin'
            #mixin what
            #mixin what
        }

        var a = 0
        doItTwice(#code {a += 1;})
        @assert(a == 2)
    }

    // When the last parameter of a mixin is of type `code`, the caller can declare that code in a statement just after the call.
    {
        #[Swag.Mixin]
        func doItTwice(value: s32, what: code)
        {
            #mixin what
            #mixin what
        }

        var a = 0
        doItTwice(4)
        {
            a += value
        }

        doItTwice(2)
        {
            a += value
        }

        @assert(a == 12)
    }

    // You can use the special name `@alias` to create a named alias for an identifier.
    {
        #[Swag.Mixin]
        func inc10()
        {
            @alias0 += 10
        }

        var a, b = 0
        inc10(|a|)  // Passing alias name 'a'
        inc10(|b|)  // Passing alias name 'b'
        @assert(a == b and b == 10)
    }

    {
        #[Swag.Mixin]
        func setVar(value: s32)
        {
            let @alias0 = value
        }

        setVar(|a| 10)  // Passing alias name 'a'
        setVar(|b| 20)  // Passing alias name 'b'
        @assert(a == 10)
        @assert(b == 20)
        setVar(30)      // No alias, so name is @alias0
        @assert(@alias0 == 30)
    }

    // You can declare special variables named `@mixin?`. Those variables will have a unique name each time the mixin is used. So the same mixin, even if it declares local variables, can be used multiple time in the same scope.
    {
        var total: s32

        #[Swag.Mixin]
        func toScope()
        {
            var @mixin0: s32 = 1
            total += @mixin0
        }

        toScope()
        toScope()
        toScope()

        @assert(total == 3)
    }
}