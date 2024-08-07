/**
A macro, like a mixin, is declared like a function, but with the attribute `Swag.Macro`.
*/
#test
{
    #[Swag.Macro]
    func myMacro() {}
}

/**
Unlike a mixin, a macro has its own scope, and cannot conflict with the function it is inserted inside.
*/
#test
{
    #[Swag.Macro]
    func myMacro()
    {
        var a = 666 // 'a' is declared in the scope of 'myMacro'
    }

    let a = 0
    myMacro() // no conflict with the 'a' declared above
    @assert(a == 0)
}

/**
But you can force an identifier to be found **outside** of the scope of the macro with `#up`.
*/
#test
{
    #[Swag.Macro]
    func myMacro()
    {
        #up a += 1 // Add '#up' before the identifier to reference the 'a' of the caller
    }

    var a = 0
    myMacro() // This will change the 'a' above
    myMacro() // This will change the 'a' above
    @assert(a == 2)
}

/**
Like a mixin, macro accepts `code` parameters.
*/
#test
{
    #[Swag.Macro]
    func myMacro(what: code)
    {
        #mixin what
    }

    var a = 0

    myMacro(#code
    {
        #up a += 1
    })

    myMacro()
    {
        #up a += 1
    }

    @assert(a == 2)
}

/**
You can use `#macro` inside a macro to force the code after to be in the same scope of the caller. That is, no `#up` is necessary to reference variables of the caller.
*/
#test
{
    #[Swag.Macro]
    func myMacro(what: code)
    {
        // No conflict, in its own scope
        var a = 666

        // Isolate the caller code, to avoid conflicts with the macro internals
        #macro
        {
            // In the scope of the caller
            #mixin #up what
        }
    }

    var a = 1
    myMacro()
    {
        a += 2 // will reference the 'a' above because this code has been inserted inside '#macro'
    }

    @assert(a == 3)
}

/**
You can extend the language with macros, without using pointers to functions (no lambda call cost).
*/
#test
{
    #[Swag.Macro]
    func repeat(count: s32, what: code)
    {
        var a = 0
        while a < count
        {
            #macro
            {
                var index = #up a
                #mixin #up what
            }

            a += 1
        }
    }

    var a = 0
    repeat(5)
    {
        a += index
    }
    @assert(a == 0 + 1 + 2 + 3 + 4)
    repeat(3)
    {
        a += index
    }
    @assert(a == 10 + 3)
}

/**
When you need `break` in the user code to break outside of a multi loop.
*/
#test
{
    #[Swag.Macro]
    func repeatSquare(count: u32, what: code)
    {
        #scope Up
        loop count
        {
            loop count
            {
                #macro
                {
                    // 'break' in the user code will be replaced by 'break Up'
                    // So it will break outside the outer loop
                    #mixin #up what { break = break Up; }
                }
            }
        }
    }

    var a = 0
    repeatSquare(5)
    {
        a += 1
        if a == 10 do
            break
    }

    @assert(a == 10)
}

/**
In a macro, you can use special variables named `#alias<num>`. Note that this is also valid for mixins.
*/
#test
{
    #[Swag.Macro]
    func call(v: s32, stmt: code)
    {
        let #alias0 = v
        let #alias1 = v * 2
        #mixin stmt
    }

    call(20)
    {
        @assert(#alias0 == 20)
        @assert(#alias1 == 40)
    }

    // The caller can then name those special variables
    // Use |name0, name1, ...| before the function call parameters
    call(|x| 20)
    {
        @assert(x == 20) // x is #alias0
        @assert(#alias1 == 40) // #alias1 is not renamed
    }

    call(|x, y| 20)
    {
        @assert(x == 20) // x is #alias0
        @assert(y == 40) // y is #alias1
    }
}
