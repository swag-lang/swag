/**
# Introduction to Swag Macros
A macro in Swag is declared similarly to a function, but with the attribute `#[Swag.Macro]`.
*/

#test
{
    #[Swag.Macro]
    func myMacro() {}
}

/**
# Macro Scope
Unlike a mixin, a macro has its own scope. This means that variables declared within the macro do not conflict with variables in the function where the macro is used.
*/
#test
{
    #[Swag.Macro]
    func myMacro()
    {
        var a = 666 // 'a' is local to the macro's scope
    }

    let a = 0
    myMacro() // No conflict with the 'a' declared in the outer scope
    @assert(a == 0) // 'a' remains unchanged
}

/**
# Resolving Identifiers Outside the Macro Scope
You can force an identifier to be resolved **outside** the macro's scope using the `#up` keyword.
*/
#test
{
    #[Swag.Macro]
    func myMacro()
    {
        #up a += 1 // References the 'a' from the outer scope
    }

    var a = 0
    myMacro() // Increment the outer 'a' by 1
    myMacro() // Increment the outer 'a' by 1 again
    @assert(a == 2) // Verify that 'a' has been incremented twice
}

/**
# Macros with Code Parameters
Like mixins, macros can accept `code` parameters, allowing the insertion of code blocks within the macro.
*/
#test
{
    #[Swag.Macro]
    func myMacro(what: code)
    {
        #mixin what // Insert the provided code block
    }

    var a = 0

    myMacro(#code
    {
        #up a += 1 // Increment 'a' within the code block
    })

    myMacro()
    {
        #up a += 1 // Another way to pass the code block directly
    }

    @assert(a == 2) // Verify that 'a' has been incremented twice
}

/**
# Forcing Code into the Caller’s Scope with `#macro`
You can use `#macro` within a macro to force the following code to be inserted in the caller's scope. This means no `#up` is necessary to reference variables from the caller.
*/
#test
{
    #[Swag.Macro]
    func myMacro(what: code)
    {
        // Declare 'a' in the macro's own scope
        var a = 666

        // Use '#macro' to ensure the following code operates in the caller's scope
        #macro
        {
            // The code block here references the caller's 'a'
            #mixin #up what
        }
    }

    var a = 1
    myMacro()
    {
        a += 2 // This 'a' references the caller's 'a' due to '#macro'
    }

    @assert(a == 3) // Verify that the caller's 'a' was incremented
}

/**
# Performance Considerations with Macros
Macros allow you to extend the language without using function pointers, avoiding the cost of lambda calls.
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
                // 'index' references 'a' from the caller's scope
                var index = #up a
                #mixin #up what
            }

            a += 1
        }
    }

    var a = 0
    repeat(5)
    {
        a += index // Sum 'index' values, where 'index' is from the macro's loop
    }
    @assert(a == 0 + 1 + 2 + 3 + 4) // Verify the sum after the first repeat

    repeat(3)
    {
        a += index // Continue summing with a new loop
    }
    @assert(a == 10 + 3) // Verify the final sum after both repeats
}

/**
# Handling Breaks in User Code with Macros
You can use `break` within user code to exit from a nested loop. This can be handled by macros using scoped loop control.
*/
#test
{
    #[Swag.Macro]
    func repeatSquare(count: u32, what: code)
    {
        #scope Up // Define a scope for breaking out of nested loops
        loop count
        {
            loop count
            {
                #macro
                {
                    // Handle 'break' in user code to break out of the outer loop
                    #mixin #up what { break = break Up; }
                }
            }
        }
    }

    var a = 0
    repeatSquare(5)
    {
        a += 1
        if a == 10:
            break // This 'break' exits the outermost loop due to the macro handling
    }

    @assert(a == 10) // Verify that the loop was exited correctly after 'a' reached 10
}

/**
# Using Aliases in Macros
In a macro, you can use special variables named `#alias<num>`, similar to mixins. These allow you to define and reference variables with specific names.
*/
#test
{
    #[Swag.Macro]
    func call(v: s32, stmt: code)
    {
        let #alias0 = v // Assign 'v' to '#alias0'
        let #alias1 = v * 2 // Assign 'v * 2' to '#alias1'
        #mixin stmt // Insert the provided code block
    }

    call(20)
    {
        @assert(#alias0 == 20) // Verify '#alias0' equals 20
        @assert(#alias1 == 40) // Verify '#alias1' equals 40
    }

    // The caller can rename these special variables by using the alias syntax
    call(|x| 20)
    {
        @assert(x == 20) // 'x' is used as an alias for '#alias0'
        @assert(#alias1 == 40) // '#alias1' is still referenced as is
    }

    call(|x, y| 20)
    {
        @assert(x == 20) // 'x' replaces '#alias0'
        @assert(y == 40) // 'y' replaces '#alias1'
    }
}
