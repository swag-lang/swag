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
Macros operate within their own scope, which is separate and isolated from the caller's scope.
This is different from mixins, which share the caller's scope.
The isolation provided by macros ensures that any variables defined inside the macro do not interfere
with or modify variables outside the macro, preventing potential naming conflicts.
*/
#test
{
    #[Swag.Macro]
    func myMacro()
    {
        // This variable 'a' is local to the macro and does not affect or interfere
        // with any 'a' outside the macro.

        var a = 666     // 'a' is confined to the macro's scope
    }

    // Declare a variable 'a' in the outer scope and initialize it to 0.
    let a = 0

    // Call the macro `myMacro()`. The macro defines its own 'a', but this does not
    // conflict with the outer 'a'.

    myMacro()          // No conflict with the outer 'a'

    // Verify that the outer 'a' remains unchanged.
    @assert(a == 0)
}

/**
# Resolving Identifiers Outside the Macro Scope
Macros typically have their own scope, which means variables inside them are isolated from the outer environment.
However, using the `#up` keyword, you can explicitly reference and modify variables that are defined outside the macro.
This allows the macro to interact with the caller's environment.
*/
#test
{
    #[Swag.Macro]
    func myMacro()
    {
        // Use `#up` to access and modify the variable `a` from the outer scope (the scope where the
        // macro is called). Without `#up`, `a` would be assumed to be a variable within the macro's
        // own scope (which might not exist).

        #up a += 1  // Increments the outer 'a' by 1
    }

    // Declare a variable `a` in the outer scope
    var a = 0

    // Call the macro `myMacro()`, which increments the outer `a` by 1 using `#up`
    myMacro()   // `a` becomes 1

    // Call the macro `myMacro()` again, which increments `a` by 1 again
    myMacro()   // `a` becomes 2

    // Verify that `a` has been incremented twice
    @assert(a == 2)
}


/**
# Macros with Code Parameters
Macros can take `code` parameters, enabling you to pass and insert code blocks dynamically within the macro.
This feature is similar to mixins but within the macro's scope.
*/
#test
{
    #[Swag.Macro]
    func myMacro(what: code void)
    {
        #inject what                            // Inserts the provided code block
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
    func myMacro(what: code void)
    {
        var a = 666                            // Declare 'a' in the macro's own scope

        #macro                                 // Ensures the following code operates in the caller's scope
        {
            #inject #up what                    // References the caller's 'a'
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
    func repeat(count: s32, what: code void)
    {
        var a = 0
        while a < count
        {
            #macro
            {
                var index = #up a              // 'index' references 'a' from the caller's scope
                #inject #up what                // Insert the provided code block
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
    func repeatSquare(count: u32, what: code void)
    {
        // Define a label `Up` for the scope that will allow us to break out of the outermost loop
        #scope ScopeTarget

        // Outer for: this will run `count` times
        for count
        {
            // Inner for: also runs `count` times
            for count
            {
                #macro
                {
                    // Injects the user code `what` here.
                    // The `#inject` directive replaces certain parts of the user code:
                    // - `break` in the user code is replaced with `break to Up`, meaning it will break
                    //   out of the `Up` scope (i.e., the outer `for`).
                    // - You can similarly redefine `continue` if needed.
                    #inject #up what => { break = break to ScopeTarget; }
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
            // This `break` statement is replaced by `break to Up` due to the macro,
            // meaning it will exit the outermost `for`, not just the inner `for`.
            break
    }

    // Assertion to check if `a` is indeed 10 after the `for` exits
    @assert(a == 10)  // Verifies that the loop exited when `a` reached 10
}

/**
## Another example:
*/
#test
{
    #[Swag.Macro]
    func repeatSquare(count: u32, what: code void)
    {
        // Define a label `Outer` for the scope that will allow us to
        // break or continue from the outermost loop
        #scope Outer

        // Outer for: this will run `count` times
        for count
        {
            // Inner for: also runs `count` times
            for count
            {
                #macro
                {
                    // `break` in the user code is replaced with `break to Outer`, exiting the outer loop.
                    // `continue` is replaced with `break`, skipping to the next iteration of the inner loop.
                    #inject #up what => { break = break to Outer; continue = break; }
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
    func call(v: s32, stmt: code void)
    {
        let #alias0 = v                        // Assign 'v' to '#alias0'
        let #alias1 = v * 2                    // Assign 'v * 2' to '#alias1'
        #inject stmt                            // Insert the provided code block
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
