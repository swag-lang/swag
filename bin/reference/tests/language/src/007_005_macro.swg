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
# Handling 'break' and 'continue' in User Code with Macros
Macros allow you to customize the behavior of `break` and `continue` statements in loops. This can be particularly useful in complex nested loops where you want a `break` or `continue` statement to affect an outer loop, not just the immediate one.

By using a macro, you can define aliases for `break` and `continue` that allow you to control exactly which loop they affect.
*/
#test
{
    #[Swag.Macro]
    func repeatSquare(count: u32, what: code)
    {
        // Define a label `Up` for the scope that will allow us to break out of the outermost loop
        #scope Up                              
        
        // Outer loop: this will run `count` times
        loop count
        {
            // Inner loop: also runs `count` times
            loop count
            {
                #macro
                {
                    // Injects the user code `what` here.
                    // The `#mixin` directive replaces certain parts of the user code:
                    // - `break` in the user code is replaced with `break Up`, meaning it will break 
                    //   out of the `Up` scope (i.e., the outer loop).
                    // - You can similarly redefine `continue` if needed.
                    #mixin #up what where { break = break Up; }
                }
            }
        }
    }

    // Initialize a variable `a` to 0
    var a = 0

    // Call the `repeatSquare` function with `count = 5`
    // The provided code block increments `a` and breaks when `a == 10`
    repeatSquare(5)
    {
        a += 1
        if a == 10:
            // This `break` statement is replaced by `break Up` due to the macro,
            // meaning it will exit the outermost loop, not just the inner loop.
            break  
    }

    // Assertion to check if `a` is indeed 10 after the loop exits
    @assert(a == 10)  // Verifies that the loop exited when `a` reached 10
}

/**
## Another example:
*/
#test
{
    #[Swag.Macro]
    func repeatSquare(count: u32, what: code)
    {
        // Define a label `Outer` for the scope that will allow us to 
        // break or continue from the outermost loop
        #scope Outer

        // Outer loop: this will run `count` times
        loop count
        {
            // Inner loop: also runs `count` times
            loop count
            {
                #macro
                {
                    // `break` in the user code is replaced with `break Outer`, exiting the outer loop.
                    // `continue` is replaced with `break`, skipping to the next iteration of the inner loop.
                    #mixin #up what where { break = break Outer; continue = break; }
                }
            }
        }
    }

    // Initialize a variable `a` to 0 and a variable `b` to 0
    var a = 0
    var b = 0

    // Call the `repeatSquare` function with `count = 5`
    // The provided code block increments `a` and uses `continue` and `break` under certain conditions
    repeatSquare(5)
    {
        a += 1

        // If `a` is divisible by 3, skip to the next iteration of the outer loop (both loops)
        if a % 3 == 0:
            continue

        // Increment `b` only if `continue` was not called
        b += 1

        // If `a` equals 8, exit both loops
        if a == 8:
            break
    }
   
    // Verifies that the loop exited when `a` reached 8
    @assert(a == 8) 

    // Verifies that `b` was incremented 6 times, skipping increments when `a` was divisible by 3
    @assert(b == 6) 
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
