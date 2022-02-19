#test
{
    // A basic test with 'if'. Curlies are optional.
    // The expression doesn't need to be enclosed with () unlike C/C++
    a := 0
    if a == 1
        @assert(false)
    if (a == 1)
        @assert(false)

    if a == 0 {
        @assert(true)
    }

    // 'else' is of course also a thing
    if a == 0
        a += 1
    else
        a += 2
    @assert(a == 1)

    // 'elif' is equivalent to 'else if'
    if a == 1
        a += 1
    else if a == 2
        @assert(false)
    elif a == 3
        @assert(false)
    elif a == 4
        @assert(false)

    // Boolean expression with 'and' and 'or'
    if a == 0 and a == 1
        @assert(false)
    if a == 0 or a == 1
        @assert(true)
}