/**
A basic test with an `if`.

In Swag, curly braces are optional for control structures like `if`, but if you omit them, you must use a colon `:`. This rule also applies to other control structures such as `while`, `for`, and `loop`.

Unlike C/C++, the condition in an `if` statement does not need to be enclosed in parentheses.
*/

#test
{
    var a = 0

    if a == 1:                    // No curly braces, so use ':'
        @assert(false)            // This block will not execute because the condition is false

    if (a == 1):                  // Parentheses can still be used if desired
        @assert(false)

    if a == 0                     // Curly braces can be used, no colon needed
    {
        @assert(true)             // This block will execute because the condition is true
    }

    // 'else' can be used as in most languages.
    // If there are no curly braces, the colon ':' is mandatory.
    if a == 0:
        a += 1                    // This will execute, making 'a' equal to 1
    else:
        a += 2
    @assert(a == 1)

    // 'elif' is used for else-if chains, similar to 'else if'.
    if a == 1:
        a += 1                    // This will execute, making 'a' equal to 2
    else:
        if a == 2:
            @assert(false)
        elif a == 3:
            @assert(false)
        elif a == 4:
            @assert(false)

    // Logical expressions with 'and' and 'or' work as expected
    if a == 0 and a == 1:         // This condition is false
        @assert(false)
    if a == 0 or a == 1:          // This condition is true (a is 1)
        @assert(true)
}

/**
# Variable declaration
In an `if` expression, you can simultaneously declare and test a variable. The use of `var`, `let`, or `const` is mandatory in this context.

The `if` statement will implicitly convert the declared variable to a boolean, evaluating it as `true` if it is non-zero. This means the `if` block will execute only if the declared variable is not zero.
*/
#test
{
    // Declare and test 'a' in the same expression.
    // Since 'a' is 0, the if block will not execute.
    // 'a' is only visible within this 'if' block.
    if let a = 0
    {
        @assert(false)
    }

    // You can redeclare 'a' as a constant in another block.
    // Since 'a' is 1, the if block will execute.
    if const a = 1:
        @assert(a == 1)
    else:
        @assert(false)

    // Another example with 'let'
    if let a = 1:
        @assert(a == 1)
    else:
        @assert(false)
}

/**
You can also add an additional condition to the `if` statement with a `where` clause. This clause is evaluated only if the initial test passes.
*/
#test
{
    func retSomething()->string => "string"
    func retNothing()->string   => null

    // The 'where' clause is checked only if 'str' is not 0.
    if let str = retSomething() where str[0] == `s`:
        @assert(true)
    else:
        @assert(false)

    // In this case, the initial test fails because 'str' is null,
    // so the 'where' clause is not evaluated.
    if let str = retNothing() where str[0] == `s`:
        @assert(false)
    else:
        @assert(true)
}
