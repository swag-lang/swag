#test
{
    // A mixin is declared like a function, with the attribute 'swag.mixin'
    {
        #[swag.mixin]
        func myMixin()
        {
        }
    }

    // A mixin function is inserted in the scope of the caller
    {
        #[swag.mixin]
        func myMixin()
        {
            a += 1
        }

        a := 0
        myMixin()
        myMixin()
        @assert(a == 2)
    }

    // This behaves like a function, so you can add parameters
    {
        #[swag.mixin]
        func myMixin(increment: s32 = 1)
        {
            a += increment
        }

        a := 0
        myMixin()
        myMixin(2)
        @assert(a == 3)
    }

    // A mixin accepts parameters of type 'code'
    {
        #[swag.mixin]
        func doItTwice(what: code)
        {
            #mixin what
            #mixin what
        }

        a := 0
        doItTwice(#code {a += 1;})
        @assert(a == 2)
    }

    // When the last parameter of a mixin is of type 'code', caller
    // can declare that code in a statement just after the call
    {
        #[swag.mixin]
        func doItTwice(value: s32, what: code)
        {
            #mixin what
            #mixin what
        }

        a := 0
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

    // You can use special type 'alias' to create a named alias
    {
        #[swag.mixin]
        func inc10(variable: alias)
        {
            variable += 10
        }

        a, b := 0
        inc10(a)
        inc10(b)
        @assert(a == b == 10)
    }
}