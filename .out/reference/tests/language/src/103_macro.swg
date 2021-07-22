#test
{
    // A macro, like a mixin, is declared like a function, but with the attribute 'Swag.Macro'
    {
        #[Swag.Macro]
        func myMacro()
        {
        }
    }

    // Unlike a mixin, a macro has its own scope, and cannot conflict with the function
    // it is inserted inside
    {
        #[Swag.Macro]
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
        #[Swag.Macro]
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
        #[Swag.Macro]
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
        #[Swag.Macro]
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

        a := 1
        myMacro()
        {
            a += 2
        }

        @assert(a == 3)
    }

    // You can extend the language with macros, without using pointers to functions
    // (no lambda call cost)
    {
        #[Swag.Macro]
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
        #[Swag.Macro]
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
        #[Swag.Macro]
        func call(v: s32, stmt: code)
        {
            @alias0 := v
            @alias1 := v * 2
            #mixin stmt
        }

        call(20)
        {
            @assert(@alias0 == 20)
            @assert(@alias1 == 40)
        }

        // The caller can then name those special variables
        // Use |name0, name1, ...| before the function call parameters
        call(|x| 20)
        {
            @assert(x == 20)        // x is @alias0
            @assert(@alias1 == 40)  // @alias1 is not renamed
        }

        call(|x, y| 20)
        {
            @assert(x == 20)   // x is @alias0
            @assert(y == 40)   // y is @alias1
        }
    }
}