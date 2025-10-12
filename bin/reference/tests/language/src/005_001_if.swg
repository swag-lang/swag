/**
# Basic Usage of 'if'

A basic test with an 'if' statement.

In Swag, curly braces `{}` are optional for control structures like `if`.
However, if you omit them, you must use `do`. This rule also applies to `while` and `for` loops.

Unlike in C/C++, the condition in an `if` statement does not need parentheses.
They can be used for clarity or grouping, but they are not required.
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

    // The 'else' keyword works as in most languages.
    // When omitting braces, 'do' is mandatory after the condition.
    if a == 0 do
        a += 1
    else do
        a += 2
    @assert(a == 1)

    // 'elif' functions like 'else if' in other languages.
    if a == 1 do
        a += 1
    else do
        if a == 2 do
            @assert(false)
        elif a == 3 do
            @assert(false)
        elif a == 4 do
            @assert(false)

    // Logical expressions work as expected with 'and' and 'or'.
    if a == 0 and a == 1 do
        @assert(false)
    if a == 0 or a == 1 do
        @assert(false)
    if a == 1 or a == 2 do
        @assert(true)
}

/**
# Variable Declaration in 'if'

You can declare and test a variable directly in an `if` statement.
When doing so, the use of `var`, `let`, or `const` is mandatory.

The declared variable is converted to a boolean for the condition:
non-zero (or non-null) values are considered `true`.
*/

#test
{
    // Declare and test 'a' in the same line.
    // Since 'a' is 0, the condition is false and the block won’t execute.
    if let a = 0
    {
        @assert(false)
    }

    // Redeclare 'a' as a constant in another scope.
    // Since 'a' is 1, the block will execute.
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

When an `if` statement includes a variable declaration, you can refine the test
with a `where` clause. The `where` condition is only evaluated if the variable test passes.
*/

#test
{
    func retSomething()->string     => "string"
    func retNothing()->#null string => null

    // The 'where' clause runs only if 'str' is not null.
    if let str = retSomething() where str[0] == 's' do
        @assert(true)
    else do
        @assert(false)

    // Since 'retNothing()' returns null, the 'where' clause is skipped.
    if let str = retNothing() where str[0] == 's' do
        @assert(false)
    else do
        @assert(true)
}
