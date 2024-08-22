/**
We have already seen that `break` is used to exit a `loop`, `visit`, `while`, `for`, and `switch`.
*/
#test
{
    loop 10:
        break                 // Exits the loop immediately
    for var i = 0; i < 10; i += 1:
        break                 // Exits the loop immediately
    while false:
        break                 // Although this line will never be reached, it demonstrates usage
}

/**
By default, `break` will exit only the innermost loop or control structure.
*/
#test
{
    var cpt = 0
    loop 10
    {
        loop 10
        {
            break             // Exits the inner loop...
        }

        // ...and continues execution here in the outer loop
        cpt += 1
    }

    @assert(cpt == 10)        // The outer loop runs 10 times
}

/**
However, you can **name a scope** using the `#scope` compiler keyword and then use `break` to exit directly to the end of that named scope.
*/
#test
{
    var cpt = 0

    // Define a scope named 'BigScope'
    #scope BigScope
    {
        loop 10
        {
            cpt += 1
            break BigScope    // Breaks out of the entire 'BigScope'
        }

        @assert(false)        // This assertion is never reached
    }

    // Execution resumes here after breaking out of 'BigScope'
    @assert(cpt == 1)         // Confirms that the loop inside 'BigScope' ran only once
}

/**
When used with a named scope, `continue` allows you to return to the beginning of the scope, effectively restarting the scope's execution.
*/
#test
{
    var cpt = 0
    #scope Loop
    {
        cpt += 1
        if cpt == 5:
            break            // Exits the 'Loop' scope when `cpt` reaches 5
        continue             // Returns to the start of 'Loop'
    }

    @assert(cpt == 5)        // Ensures the loop runs exactly 5 times
}

/**
It's not mandatory to name the scope, so this technique can be used as an alternative to a series of if/else statements, providing a clean and structured way to manage multiple conditions.
*/
#test
{
    let cpt = 0
    #scope
    {
        if cpt == 1
        {
            @assert(cpt == 1)
            break            // Exits the unnamed scope
        }

        if cpt == 2
        {
            @assert(cpt == 2)
            break            // Exits the unnamed scope
        }

        if cpt == 3
        {
            @assert(cpt == 3)
            break            // Exits the unnamed scope
        }
    }
}

/**
Note that a scope can be followed by a simple statement rather than a block, allowing for flexible control over flow.
*/
#test
{
    #scope Up
    loop 10
    {
        loop 10
        {
            if #index == 5:
                break Up      // Exits to the 'Up' scope
        }

        @assert(false)        // This assertion is never reached
    }
}
