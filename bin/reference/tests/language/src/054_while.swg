/**
`while` is a loop that runs **until the expression is false**.
*/
#test
{
    var i = 0
    while i < 10:
        i += 1
    @assert(i == 10)
}

/**
You can also `break` and `continue` inside a `while`.
*/
#test
{
    var i = 0
    while i < 10
    {
        if i == 5:
            break
        i += 1
    }

    @assert(i == 5)
}
