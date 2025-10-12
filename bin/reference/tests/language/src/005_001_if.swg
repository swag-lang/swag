/**
# Basic Usage of 'if'
A basic test with an 'if' statement.

In Swag, curly braces '{}' are optional for control structures like 'if'. However, if you choose to omit them, you must use 'do'. This syntax rule also applies to other control structures such as 'while' and 'for'.

Unlike in C/C++, the condition in an 'if' statement does not need to be enclosed in parentheses. Parentheses can be used for clarity or grouping, but they are not mandatory.
*/

#test
{
    var a = 0

    if a == 1 do
        @assert(false)

    if (a == 1) do
        @assert(false)

    if a == 0
    {
        @assert(true)
    }

    // The 'else' keyword can be used as in most programming languages.
    // When not using curly braces, the 'do' after the condition is mandatory.
    if a == 0 do
        a += 1
    else do
        a += 2
    @assert(a == 1)

    // The 'elif' keyword is used for else-if chains, functioning similarly to 'else if' in other languages.
    if a == 1 do
        a += 1
    else do
        if a == 2 do
            @assert(false)
        elif a == 3 do
            @assert(false)
        elif a == 4 do
            @assert(false)

    // Logical expressions using 'and' and 'or' work as expected
    if a == 0 and a == 1 do
        @assert(false)
    if a == 0 or a == 1 do
        @assert(false)
    if a == 1 or a == 2 do
        @assert(true)
}

/**
# Variable Declaration in 'if'
You can declare and test a variable in an 'if' statement simultaneously. In this context, the use of 'var', 'let', or 'const' is mandatory.

The condition in the 'if' statement will automatically convert the declared variable to a boolean. If the variable is non-zero, the condition evaluates to 'true', and the 'if' block will execute.
*/

#test
{
    // Declare and test 'a' within the 'if' statement. Since 'a' is 0, the 'if' block will not execute.
    // The variable 'a' is only scoped within this 'if' block.
    if let a = 0
    {
        @assert(false)
    }

    // You can redeclare 'a' as a constant in another block.
    // Since 'a' is 1, the 'if' block will execute.
    if const a = 1 do
        @assert(a == 1)
    else do
        @assert(false)

    if let a = 1 do
        @assert(a == 1)
    else do
        @assert(false)
}

/**
# Adding Conditions with 'where'
When an 'if' statement includes a variable declaration, you can add an additional condition using a 'where' clause. The 'where' clause is evaluated only if the initial test passes.
*/

#test
{
    func retSomething()->string     => "string"
    func retNothing()->#null string => null

    // The 'where' clause is only evaluated if 'str' is not null.
    if let str = retSomething() where str[0] == 's' do
        @assert(true)
    else do
        @assert(false)

    // In this example, the initial test fails because 'str' is null,
    // so the 'where' clause is not evaluated.
    if let str = retNothing() where str[0] == 's' do
        @assert(false)
    else do
        @assert(true)
}
