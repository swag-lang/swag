/**
A basic test with `if`. Curlies are optional, but in that case you need to use 'do'.
This will be the same rule for 'while', 'for', 'loop' and so on.
Unlike C/C++, the expression doesn't need to be enclosed with `()`.
*/

#test
{
    var a = 0
    if a == 1 do
        @assert(false)
    if (a == 1) do
        @assert(false)
    if a == 0 {
        @assert(true)
    }

    // 'else' is of course also a thing.
    // 'do' is mandatory if the statement is not in a curly block.
    if a == 0 do
        a += 1
    else do
        a += 2
    @assert(a == 1)

    // 'elif' is equivalent to 'else do if'
    if a == 1 do
        a += 1
    else do if a == 2 do
        @assert(false)
    elif a == 3 do
        @assert(false)
    elif a == 4 do
        @assert(false)

    // Boolean expression with 'and' and 'or'
    if a == 0 and a == 1 do
        @assert(false)
    if a == 0 or a == 1 do
        @assert(true)
}

/**
You can also at the same time declare and test one variable in an `if` expression. `var`, `let` or `const` is mandatory in that case.
*/
#test
{
    // This will declare a variable 'a', and test it against 0.
    // 'a' is then only visible in the 'if' block, and not outside.
    if var a = 0
    {
        @assert(false)
    }

    // So you can redeclare 'a' (this time as a constant).
    if const a = 1 do
        @assert(a == 1)
    else do
        @assert(false)

    if let a = 1 do
        @assert(a == 1)
    else do
        @assert(false)
}