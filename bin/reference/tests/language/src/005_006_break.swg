/**
# Introduction to 'break' in Swag
The 'break' statement exits the nearest enclosing control structure.
It works with 'for', 'foreach', 'while', and 'switch'. Use it to stop a loop
early or to leave a switch case.
*/
#test
{
    // Immediately exits the loop body
    for 10 do
        break

    // Exits after the first iteration
    for var i = 0; i < 10; i += 1 do
        break

    // Not reached because the condition is false
    while false do
        break
}

/**
# Default Behavior
By default, 'break' exits only the innermost loop or control structure.
Outer structures continue to run.
*/
#test
{
    var cpt = 0

    for 10
    {
        for 10
        {
            // Exits only the inner loop
            break
        }
        // Outer loop continues
        cpt += 1
    }

    @assert(cpt == 10)
}

/**
# Named Scopes with '#scope'
Define a named scope with '#scope(Name)'. 'break to Name' exits that scope
from anywhere inside it.
*/
#test
{
    var cpt = 0

    #scope(BigScope)
    {
        for 10
        {
            cpt += 1
            break to BigScope // Leave the whole 'BigScope'
        }

        @assert(false) // Unreachable
    }

    @assert(cpt == 1)
}

/**
# Using 'continue' with Named Scopes
Within a scope, 'continue' restarts execution from the beginning of that scope.
Combine a terminating condition with a plain 'break' to end the scope loop.
*/
#test
{
    var cpt = 0

    #scope(Loop)
    {
        cpt += 1
        if cpt == 5 do
            break // End the 'Loop' scope
        continue // Jump back to the start of 'Loop'
    }

    @assert(cpt == 5)
}

/**
# Unnamed Scopes
Scopes can be unnamed. 'break' exits the current (unnamed) scope immediately.
This can simplify multi-branch flows.
*/
#test
{
    let cpt = 0

    #scope
    {
        if cpt == 1
        {
            @assert(cpt == 1)
            break
        }

        if cpt == 2
        {
            @assert(cpt == 2)
            break
        }

        if cpt == 3
        {
            @assert(cpt == 3)
            break
        }
    }
}

/**
# Scopes with Simple Statements
A scope label can precede a simple statement. 'break to Label' exits to just
after that labeled statement.
*/
#test
{
    #scope(Up)
    for 10
    {
        for 10
        {
            if @index == 5 do
                break to Up // Exit to after the labeled 'for'
        }

        @assert(false) // Unreachable
    }
}
