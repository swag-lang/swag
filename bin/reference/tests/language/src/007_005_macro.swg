/**
# Introduction to Swag Macros
Macros in Swag are declared like functions, but with the '#[Swag.Macro]' attribute.
They are expanded at compile time and can be reused to generate code.
*/

#test
{
    #[Swag.Macro]
    func myMacro() {}
}

/**
# Macro Scope
Macros run in their own scope, isolated from the caller. Unlike mixins, they do not
share the caller's scope. Variables defined inside a macro cannot interfere with
the caller unless explicitly elevated.
*/
#test
{
    #[Swag.Macro]
    func myMacro()
    {
        // 'a' here is local to the macro
        var a = 666
    }

    let a = 0
    myMacro() // no conflict with outer 'a'
    @assert(a == 0)
}

/**
# Resolving Identifiers Outside the Macro Scope
Use '#up' to explicitly reference and modify variables from the caller's scope.
*/
#test
{
    #[Swag.Macro]
    func myMacro()
    {
        #up a += 1 // increments the caller's 'a'
    }

    var a = 0
    myMacro()
    myMacro()
    @assert(a == 2)
}

/**
# Macros with '#code' Parameters
Macros can accept '#code' parameters to inject caller-provided code.
*/
#test
{
    #[Swag.Macro]
    func myMacro(what: #code void)
    {
        #inject(what) // insert provided code
    }

    var a = 0

    // Use '#code' to pass a block or expression as code (not evaluated at call site)
    myMacro(#code { #up a += 1; })

    // If the last parameter is '#code void', the following statement becomes the last argument
    myMacro()
    {
        #up a += 1
    }

    @assert(a == 2)
}

/**
# Typed '#code' Parameters
A '#code' parameter can be typed:
- Use '#code void' for a code statement.
- Use '#code <type>' for a code expression that returns a specific type.
*/
#test
{
    #[Swag.Macro]
    func myMacro(what: #code bool)
    {
        @assert(#inject(what))
    }

    // The expression is transformed to code and evaluated only after injection
    myMacro(1 == 1)
    myMacro(3 > 2 and 2 < 4)
    myMacro(true)

    // Note: '#code' creates an untyped piece of code usable with any argument type
    myMacro(#code true)
}

/**
# Forcing Code into the Caller’s Scope with '#macro'
Use '#macro' to run injected code as if it were in the caller's scope,
avoiding the need for '#up' inside the injected block.
*/
#test
{
    #[Swag.Macro]
    func myMacro(what: #code void)
    {
        var a = 666 // macro-local 'a'

        #macro
        {
            // references the caller's scope
            #inject(#up what)
        }
    }

    var a = 1
    myMacro()
    {
        a += 2 // operates on caller's 'a' due to '#macro'
    }

    @assert(a == 3)
}

/**
# Performance Considerations with Macros
Macros extend the language without function-pointer/lambda overhead.
They can be used to generate tight loops with caller-visible indices, etc.
*/
#test
{
    #[Swag.Macro]
    func repeat(count: s32, what: #code void)
    {
        var a = 0
        while a < count
        {
            #macro
            {
                var index = #up a // 'index' visible in caller via macro scope
                #inject(#up what)
                // insert caller code
            }
            a += 1
        }
    }

    var a = 0
    repeat(5)
    {
        a += index // sum 0..4
    }
    @assert(a == 0 + 1 + 2 + 3 + 4)

    repeat(3)
    {
        a += index // add 0..2
    }
    @assert(a == 10 + 3)
}

/**
# Handling 'break' and 'continue' in User Code with Macros
You can remap 'break'/'continue' in injected user code to control which loop they target.
*/
#test
{
    #[Swag.Macro]
    func repeatSquare(count: u32, what: #code void)
    {
        // Define a scope label used as the break target
        #scope(ScopeTarget)

        for count
        {
            for count
            {
                #macro
                {
                    // Remap 'break' in user code to 'break to ScopeTarget' (outer loop exit)
                    #inject(#up what, break = break to ScopeTarget)
                }
            }
        }
    }

    var a = 0

    repeatSquare(5)
    {
        a += 1
        if a == 10 do
            break // remapped to 'break to ScopeTarget'
    }

    @assert(a == 10)
}

/**
## Another example:
Remap both 'break' and 'continue' to influence outer/inner loop flow.
*/
#test
{
    #[Swag.Macro]
    func repeatSquare(count: u32, what: #code void)
    {
        // Label for controlling the outer loop
        #scope(Outer)

        for count
        {
            for count
            {
                #macro
                {
                    // 'break' -> exit outer loop; 'continue' -> skip inner iteration
                    #inject(#up what, break = break to Outer, continue = break)
                }
            }
        }
    }

    var a = 0
    var b = 0

    repeatSquare(5)
    {
        a += 1

        // If 'a' divisible by 3, skip to next inner iteration (acts like 'continue')
        if a % 3 == 0 do
            continue

        b += 1

        // Exit both loops when 'a' reaches 8
        if a == 8 do
            break
    }

    @assert(a == 8)
    @assert(b == 6)
}

/**
# Using Aliases in Macros
Special variables '#alias<num>' inside a macro can be overridden with named aliases
at the call site for clearer code.
*/
#test
{
    #[Swag.Macro]
    func call(v: s32, stmt: #code void)
    {
        let #alias0 = v
        let #alias1 = v * 2
        #inject(stmt)
    }

    call(20)
    {
        @assert(#alias0 == 20)
        @assert(#alias1 == 40)
    }

    call(|x| 20)
    {
        @assert(x == 20) // 'x' aliases '#alias0'
        @assert(#alias1 == 40) // '#alias1' unchanged
    }

    call(|x, y| 20)
    {
        @assert(x == 20) // replaces '#alias0'
        @assert(y == 40) // replaces '#alias1'
    }
}
