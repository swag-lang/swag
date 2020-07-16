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

    // This is like a function, so you can add parameters
    {
        #[swag.mixin]
        func myMixin(increment: s32)
        {
            a += increment
        }

        a := 0
        myMixin(1)
        myMixin(2)
        @assert(a == 3)
    }
}