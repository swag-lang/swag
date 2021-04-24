#test
{
    // A macro, like a mixin, is declared like a function, but with the attribute 'swag.macro'
    {
        #[swag.macro]
        func myMacro()
        {
        }
    }

    // Unlike a mixin, a macro has its own scope, and cannot conflict with the function
    // it is inserted inside
    {
        #[swag.macro]
        func myMacro()
        {
            a := 666
        }

        a := 0
        myMacro()
        myMacro()
        @assert(a == 0)
    }

    // But you can force an identifier to be found outside of the scope of the macro
    {
        #[swag.macro]
        func myMacro()
        {
            `a += 1 // Back ticked to reference the 'a' of the caller
        }

        a := 0
        myMacro()
        myMacro()
        @assert(a == 2)
    }

    // Like a mixin, macro accepts 'code' parameters
    {
        #[swag.macro]
        func myMacro(what: code)
        {
            #mixin what
        }

        a := 0

        myMacro(#code { `a += 1; } )

        myMacro()
        {
            `a += 1
        }

        @assert(a == 2)
    }

    // You can use #macro inside a macro to force the code after to be in the
    // same scope of the caller. That is, no back tick is necessary to reference
    // variables of the caller.
    {
        #[swag.macro]
        func myMacro(what: code)
        {
            // No conflict, in its own scope
            a := 0

            #macro
            {
                // In the scope of the caller
                #mixin `what
            }
        }

        a := 0
        myMacro()
        {
            a += 2
        }

        @assert(a == 2)
    }

    // You can extend the language with macros
    {
        #[swag.macro]
        func repeat(count: s32, what: code)
        {
            a := 0
            while a < count
            {
                #macro
                {
                    var index = `a
                    #mixin `what
                }

                a += 1
            }
        }

        a := 0
        repeat(5) { a += index; }
        @assert(a == 0+1+2+3+4)
        repeat(3) { a += index; }
        @assert(a == 10+3)
    }

    // When you need 'break' in the user code to break outside of
    // a multi loop
    {
        #[swag.macro]
        func repeatSquare(count: u32, what: code)
        {
            label up loop count
            {
                loop count
                {
                    #macro
                    {
                        #mixin `what { break = break up; }
                    }
                }
            }
        }

        a := 0
        repeatSquare(5)
        {
            a += 1
            if a == 10
                break
        }

        @assert(a == 10)
    }

    // In a macro, you can use special variables named '@alias<num>'
    // Note that this is also valid for mixins.
    {
        #[swag.macro]
        func call(stmt: code)
        {
            @alias0 := 444
            @alias1 := 555
            #mixin stmt
        }

        // The caller can then name those special variables
        call() <- |x, y|
        {
            @assert(x == 444)   // x is @alias0
            @assert(y == 555)   // y is @alias1
        }
    }
}