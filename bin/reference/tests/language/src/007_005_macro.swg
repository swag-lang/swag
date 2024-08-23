/**
# Introduction to Swag Macros
Macros in Swag are defined similarly to functions, with the key distinction being the `#[Swag.Macro]` attribute.
This attribute indicates that the function is intended to be a macro, which can be reused and expanded at compile-time.
*/

#test
{
    #[Swag.Macro]
    func myMacro() {}
}

/**
# Macro Scope
Macros operate within their own scope, unlike mixins that share the scope of their caller. 
This isolation ensures that variables within a macro do not interfere with those outside it, preventing conflicts.
*/
#test
{
    #[Swag.Macro]
    func myMacro()
    {
        var a = 666                            // 'a' is confined to the macro's scope
    }

    let a = 0
    myMacro()                                  // No conflict with the outer 'a'
    @assert(a == 0)                            // 'a' remains as defined outside the macro
}

/**
# Resolving Identifiers Outside the Macro Scope
Using the `#up` keyword, you can explicitly reference variables from outside the macro's scope.
This allows a macro to interact with variables in the caller's environment.
*/
#test
{
    #[Swag.Macro]
    func myMacro()
    {
        #up a += 1                             // References 'a' from the outer scope
    }

    var a = 0
    myMacro()                                  // Increments the outer 'a' by 1
    myMacro()                                  // Increments the outer 'a' by 1 again
    @assert(a == 2)                            // Verifies that 'a' has been incremented twice
}

/**
# Macros with Code Parameters
Macros can take `code` parameters, enabling you to pass and insert code blocks dynamically within the macro.
This feature is similar to mixins but within the macro's scope.
*/
#test
{
    #[Swag.Macro]
    func myMacro(what: code)
    {
        #mixin what                            // Inserts the provided code block
    }

    var a = 0

    myMacro(#code
    {
        #up a += 1                             // Increment 'a' within the code block
    })

    myMacro()
    {
        #up a += 1                             // Alternative way to pass the code block directly
    }

    @assert(a == 2)                            // Verifies that 'a' has been incremented twice
}

/**
# Forcing Code into the Caller’s Scope with `#macro`
The `#macro` keyword can be used to ensure that the code within a macro operates in the caller's scope.
This technique negates the need for the `#up` keyword when referencing the caller's variables.
*/
#test
{
    #[Swag.Macro]
    func myMacro(what: code)
    {
        var a = 666                            // Declare 'a' in the macro's own scope

        #macro                                 // Ensures the following code operates in the caller's scope
        {
            #mixin #up what                    // References the caller's 'a'
        }
    }

    var a = 1
    myMacro()
    {
        a += 2                                 // 'a' references the caller's 'a' due to '#macro'
    }

    @assert(a == 3)                            // Verifies that the caller's 'a' was incremented
}

/**
# Performance Considerations with Macros
Macros in Swag can extend the language capabilities without relying on function pointers.
This avoids the overhead associated with lambda calls, making macros a performance-efficient choice.
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
                var index = #up a              // 'index' references 'a' from the caller's scope
                #mixin #up what                // Insert the provided code block
            }
            a += 1
        }
    }

    var a = 0
    repeat(5)
    {
        a += index                             // Sum 'index' values from the macro's loop
    }
    @assert(a == 0 + 1 + 2 + 3 + 4)            // Verifies the sum after the first repeat

    repeat(3)
    {
        a += index                             // Continue summing with a new loop
    }
    @assert(a == 10 + 3)                       // Verifies the final sum after both repeats
}

/**
# Handling Breaks in User Code with Macros
Macros can handle user-defined `break` statements, allowing for controlled exits from nested loops.
This is particularly useful for complex loops where break conditions are needed.
*/
#test
{
    #[Swag.Macro]
    func repeatSquare(count: u32, what: code)
    {
        #scope Up                              // Define a scope for handling loop breaks
        loop count
        {
            loop count
            {
                #macro
                {
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
            break                              // This 'break' exits the outermost loop
    }

    @assert(a == 10)                           // Verifies loop exit after 'a' reached 10
}

/**
# Using Aliases in Macros
Special variables named `#alias<num>` can be used within macros, similar to mixins.
These aliases allow you to define and access specific variables within a macro.
*/
#test
{
    #[Swag.Macro]
    func call(v: s32, stmt: code)
    {
        let #alias0 = v                        // Assign 'v' to '#alias0'
        let #alias1 = v * 2                    // Assign 'v * 2' to '#alias1'
        #mixin stmt                            // Insert the provided code block
    }

    call(20)
    {
        @assert(#alias0 == 20)                 // Verifies '#alias0' equals 20
        @assert(#alias1 == 40)                 // Verifies '#alias1' equals 40
    }

    call(|x| 20)
    {
        @assert(x == 20)                       // 'x' is used as an alias for '#alias0'
        @assert(#alias1 == 40)                 // '#alias1' remains unchanged
    }

    call(|x, y| 20)
    {
        @assert(x == 20)                       // 'x' replaces '#alias0'
        @assert(y == 40)                       // 'y' replaces '#alias1'
    }
}
