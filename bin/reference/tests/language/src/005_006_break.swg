/**
# Introduction to `break` in Swag
The `break` statement is a powerful control structure in Swag, allowing you to exit from `loop`, `visit`, `while`, `for`, and `switch` constructs. Understanding how and when to use `break` is essential for effective flow control in your programs.
*/
#test
{
    loop 10:
        break                 // Immediately exits the loop, no further iterations occur

    for var i = 0; i < 10; i += 1:
        break                 // Exits the loop instantly after the first iteration

    while false:
        break                 // Demonstrates usage, but this line is never reached due to the false condition
}

/**
# Default Behavior of `break`
By default, the `break` statement exits only the innermost loop or control structure in which it resides. This ensures that nested loops or structures can be controlled independently.
*/
#test
{
    var cpt = 0
    loop 10
    {
        loop 10
        {
            break             // Exits the inner loop only, allowing the outer loop to continue
        }

        // The outer loop continues its execution here
        cpt += 1
    }

    @assert(cpt == 10)        // Verifies that the outer loop has run exactly 10 times
}

/**
# Named Scopes with `break`
Swag allows you to define named scopes using the `#scope` keyword. The `break` statement can then be directed to exit from the named scope, offering a structured approach to control flow within complex nested loops or conditions.
*/
#test
{
    var cpt = 0

    // Defining a scope named 'BigScope'
    #scope BigScope
    {
        loop 10
        {
            cpt += 1
            break in BigScope    // Exits the entire 'BigScope' scope after incrementing `cpt`
        }

        @assert(false)        // This assertion is never reached due to the break in statement above
    }

    // Execution resumes after 'BigScope'
    @assert(cpt == 1)         // Confirms that the loop inside 'BigScope' executed only once
}

/**
# Using `continue` with Named Scopes
The `continue` statement can be used with a named scope to restart the execution from the beginning of the scope. This can be useful for iterating until a specific condition is met.
*/
#test
{
    var cpt = 0

    #scope Loop
    {
        cpt += 1
        if cpt == 5:
            break            // Exits the 'Loop' scope when `cpt` equals 5, ending the loop
        continue             // Restarts the 'Loop' scope, incrementing `cpt` each time
    }

    @assert(cpt == 5)        // Verifies that the loop runs exactly 5 times before exiting
}

/**
# Unnamed Scopes for Flow Control
Named scopes are optional. You can also use unnamed scopes to manage control flow, offering a structured alternative to traditional if/else statements. This can help simplify complex conditions.
*/
#test
{
    let cpt = 0
    #scope
    {
        if cpt == 1
        {
            @assert(cpt == 1)
            break            // Exits the unnamed scope if `cpt` equals 1
        }

        if cpt == 2
        {
            @assert(cpt == 2)
            break            // Exits the unnamed scope if `cpt` equals 2
        }

        if cpt == 3
        {
            @assert(cpt == 3)
            break            // Exits the unnamed scope if `cpt` equals 3
        }
    }
}

/**
# Using `break` with Simple Statements
A scope in Swag can also be followed by a simple statement rather than a block, providing additional flexibility in controlling the flow of execution.
*/
#test
{
    #scope Up
    loop 10
    {
        loop 10
        {
            if #index == 5:
                break in Up      // Exits to the 'Up' scope when the inner loop index equals 5
        }

        @assert(false)        // This assertion is never reached due to the break in statement above
    }
}
