/**
# Basic Usage of `if`
A basic test with an `if` statement.

In Swag, curly braces `{}` are optional for control structures like `if`. However, if you choose to omit them, you must use a colon `:`. This syntax rule also applies to other control structures such as `while` and `for`.

Unlike in C/C++, the condition in an `if` statement does not need to be enclosed in parentheses. Parentheses can be used for clarity or grouping, but they are not mandatory.
*/

#test
{
    var a = 0

    if a == 1:                    // No curly braces, so a colon ':' is required after the condition
        @assert(false)            // This block will not execute since 'a' is not equal to 1

    if (a == 1):                  // Parentheses can be used for grouping or clarity, but are optional
        @assert(false)            // This block will also not execute

    if a == 0                     // Curly braces are used, no colon is needed here
    {
        @assert(true)             // This block will execute because 'a' equals 0
    }

    // The 'else' keyword can be used as in most programming languages.
    // When not using curly braces, the colon ':' after the condition is mandatory.
    if a == 0:
        a += 1                    // This block will execute, setting 'a' to 1
    else:
        a += 2                    // This block is skipped because the 'if' condition is true
    @assert(a == 1)               // Asserts that 'a' is now 1

    // The 'elif' keyword is used for else-if chains, functioning similarly to 'else if' in other languages.
    if a == 1:
        a += 1                    // This block will execute, making 'a' equal to 2
    else:
        if a == 2:                // This nested 'if' will not execute
            @assert(false)
        elif a == 3:              // This condition is also false
            @assert(false)
        elif a == 4:              // This condition is false as well
            @assert(false)

    // Logical expressions using 'and' and 'or' work as expected
    if a == 0 and a == 1:         // This condition is false because 'a' cannot be both 0 and 1
        @assert(false)            // This block will not execute
    if a == 0 or a == 1:          // This condition is false because 'a' is 2
        @assert(false)
    if a == 1 or a == 2:          // This condition is true because 'a' is 2
        @assert(true)             // This block will execute
}

/**
# Variable Declaration in `if`
You can declare and test a variable in an `if` statement simultaneously. In this context, the use of `var`, `let`, or `const` is mandatory.

The condition in the `if` statement will automatically convert the declared variable to a boolean. If the variable is non-zero, the condition evaluates to `true`, and the `if` block will execute.
*/

#test
{
    // Declare and test 'a' within the 'if' statement. Since 'a' is 0, the 'if' block will not execute.
    // The variable 'a' is only scoped within this 'if' block.
    if let a = 0
    {
        @assert(false)            // This block will not execute
    }

    // You can redeclare 'a' as a constant in another block.
    // Since 'a' is 1, the 'if' block will execute.
    if const a = 1:
        @assert(a == 1)           // This block will execute and assert that 'a' is 1
    else:
        @assert(false)            // This block will not execute

    // Another example using 'let'
    if let a = 1:
        @assert(a == 1)           // This block will execute, confirming that 'a' is 1
    else:
        @assert(false)            // This block will not execute
}

/**
# Adding Conditions with `where`
When an `if` statement includes a variable declaration, you can add an additional condition using a `where` clause. The `where` clause is evaluated only if the initial test passes.
*/

#test
{
    func retSomething()->string => "string"
    func retNothing()->string   => null

    // The 'where' clause is only evaluated if 'str' is not null.
    if let str = retSomething() where str[0] == `s`:
        @assert(true)             // This block will execute since 'str' starts with 's'
    else:
        @assert(false)            // This block will not execute

    // In this example, the initial test fails because 'str' is null,
    // so the 'where' clause is not evaluated.
    if let str = retNothing() where str[0] == `s`:
        @assert(false)            // This block will not execute
    else:
        @assert(true)             // This block will execute since the initial test fails
}
