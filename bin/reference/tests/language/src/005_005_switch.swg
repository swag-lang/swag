/**
# Introduction to `switch` in Swag
The `switch` statement in Swag operates similarly to those in C/C++, with a key distinction: Swag does not require an explicit `break` statement at the end of each `case` block. Instead, it prevents unintentional `fallthrough` behavior by design, except when the `case` is empty, where a `break` statement is necessary. This ensures that each `case` is independent unless explicitly designed otherwise, reducing the risk of errors in control flow.
*/

#test
{
    let value = 6

    // The `switch` statement evaluates the value of 'value'.
    // The corresponding `case` block is executed based on the match.
    switch value
    {
    case 0:
        @assert(false) // Assertion fails if 'value' is 0.
    case 5:
        @assert(false) // Assertion fails if 'value' is 5.
    case 6:
        @assert(true) // Assertion passes if 'value' is 6.
    default:
        @assert(false) // Assertion fails if no cases match.
    }

    let ch = 'A''rune

    // This `switch` checks the value of 'ch'.
    switch ch
    {
    case 'B':
        @assert(false) // Assertion fails if 'ch' is 'B'.
    case 'A':
        break // Exits the switch when 'ch' is 'A'.
    }
}

/**
# Multiple Values in a `case`
Swag allows you to assign multiple values to a single `case` statement, simplifying the code when the same block of code should execute for several potential matches. This feature enhances the readability and maintainability of your switch statements.
*/

#test
{
    let value = 6

    // In this switch, the `case` statement handles multiple values.
    switch value
    {
    case 2, 4, 6:
        @assert(true) // Assertion passes if 'value' is 2, 4, or 6.
    default:
        @assert(false) // Assertion fails if 'value' does not match any case.
    }

    // Listing each value on its own line for enhanced readability.
    switch value
    {
    case 2,
         4,
         6:
        @assert(true) // Assertion passes for any listed value.
    default:
        @assert(false) // Assertion fails if no cases match.
    }
}

/**
# Using `switch` with Various Types
The `switch` statement in Swag is versatile, supporting any type that implements the `==` operator. This flexibility extends beyond numeric types to include strings and other comparable types, making it a powerful tool for various decision-making scenarios.
*/

#test
{
    let value = "myString"

    // A `switch` statement can match string values as well.
    switch value
    {
    case "myString":
        @assert(true) // Assertion passes if 'value' matches "myString".
    case "otherString":
        @assert(false) // Assertion fails if 'value' matches "otherString".
    default:
        @assert(false) // Assertion fails if no cases match.
    }
}

/**
# Intentional Fallthrough with `fallthrough`
Swag allows for intentional `fallthrough` behavior, similar to C/C++, using the `fallthrough` keyword. This feature provides the option to continue execution from one `case` block to the next, which can be useful in scenarios where multiple cases share common logic.
*/

#test
{
    let value = 6

    // Demonstrating fallthrough where execution continues from `case 6` to `case 7`.
    switch value
    {
    case 6:
        fallthrough // Explicitly continues to the next `case`.
    case 7:
        @assert(value == 6) // Assertion passes, confirming 'value' is 6.
    default:
        @assert(true) // Default case is a safeguard; true assertion keeps the test passing.
    }
}

/**
# Exiting a `case` Early with `break`
The `break` statement in Swag allows for early exit from a `case` block. This is particularly useful when only a portion of the block should be executed under specific conditions, enhancing control flow within your `switch` statements.
*/

#test
{
    let value = 6

    // Demonstrating the use of `break` to exit the switch early.
    switch value
    {
    case 6:
        if value == 6 do
            break // Exits the switch if the condition is true.
        @assert(false) // This line is never reached if `value` is 6.
    default:
        @assert(false) // Assertion fails if no cases match.
    }
}

/**
# Handling Empty Cases with `break`
In Swag, a `case` statement cannot be left empty. If no action is required for a particular `case`, the `break` statement must be used explicitly to avoid compilation errors, ensuring clarity in control flow.
*/

#test
{
    let value = 6

    // Handling cases where no action is needed using `break`.
    switch value
    {
    case 5:
        @assert(false) // Assertion fails if `value` is 5.
    case 6:
        break // No action is taken; switch exits cleanly.
    default:
        @assert(false) // Assertion fails if no cases match.
    }
}

/**
# Variable and Expression Cases
Swag’s `switch` statement offers the flexibility to use variables and expressions in `case` conditions. This allows for dynamic evaluation based on runtime values, further extending the utility of the `switch` statement.
*/

#test
{
    let test = 2
    let a    = 0
    let b    = 1

    // Demonstrating the use of variables and expressions in switch cases.
    switch test
    {
    case a:
        @assert(false) // Assertion fails if `test` equals `a` (0).
         // `a` is a variable here, not a constant.
    case b:
        @assert(false) // Assertion fails if `test` equals `b` (1).
         // `b` is another variable.
    case b + 1:
        @assert(true) // Assertion passes if `test` equals `b + 1` (2).
         // This case uses an expression.
    }
}

/**
# The `Swag.Complete` Attribute
The `Swag.Complete` attribute ensures exhaustive handling of all possible cases in an enum within a `switch` statement. If any enum value is not explicitly handled, the compiler will raise an error, enforcing robust and complete logic coverage.
*/

#test
{
    enum Color { Red, Green, Blue }
    let color = Color.Red

    #[Swag.Complete]
    switch color
    {
    case Color.Red:
        break // Handles the `Red` case.
    case Color.Green:
        @assert(false) // Assertion fails if `color` is Green.
    case Color.Blue:
        @assert(false) // Assertion fails if `color` is Blue.
    }
}

/**
# Matching Ranges in a `switch` Statement
Swag supports matching a **range of values** in a `switch` statement, allowing you to group and handle multiple values that fall within a specific range efficiently. This is useful for concise and clear range-based logic within your code.
*/

#test
{
    var success = false
    let x       = 6

    // Switch statement using ranges to match a group of values.
    switch x
    {
    case 0 to 5:
        @assert(false) // Assertion fails if `x` is between 0 and 5.
    case 6 to 15:
        success = true // Sets `success` to true if `x` is between 6 and 15.
    }

    @assert(success) // Ensures the correct case was matched.
}

/**
# Overlapping Ranges
In Swag, if ranges within a `switch` statement overlap, the first valid range that matches will be executed, while subsequent overlapping ranges are ignored. This order of precedence must be carefully considered in your logic.
*/

#test
{
    var success = false
    let x       = 6

    // Demonstrating overlapping ranges and their precedence.
    switch x
    {
    case 0 to 10:
        success = true // This case matches first; subsequent cases are ignored.
    case 5 until 15:
        @assert(false) // This is not reached because the first range matches.
    }

    @assert(success) // Confirms that the first range was applied correctly.
}

/**
# Using the `where` Clause in `switch`
The `where` clause in Swag allows you to add additional conditions to a `case` in a `switch` statement. This provides finer control over the logic, enabling complex decision-making scenarios where multiple variables influence the outcome.
*/

#test
{
    let x = 6
    let y = 10

    // Each `case` checks the value of 'x' and applies an additional condition with 'where'.
    switch x
    {
    case 6 where y == 9:
        @assert(false) // Skipped because `y` is not 9.
    case 6 where y == 10:
        @assert(true) // Executes because both `x` and `y` match the conditions.
    default:
        @assert(false) // Fallback case; should not be executed here.
    }
}

/**
# Using `where` with `default`
The `where` clause can also be applied to a `default` case, providing conditional logic even when no specific `case` matches. This allows you to handle default scenarios with more precision, based on additional criteria.
*/

#test
{
    let x = 7
    let y = 10

    // Demonstrates different outcomes based on 'where' clauses in default.
    switch x
    {
    case 6 where y == 10:
        @assert(false) // Skipped because `x` is not 6.
    case 7 where y == 9:
        @assert(false) // Skipped because `y` is not 9.
    default where y == 10:
        break // Executes because `y` is 10 and no previous case matched.
    default:
        @assert(false) // General fallback case; should not be executed.
    }
}

/**
# Switching on Type with `any` or `interface`
When using a `switch` statement with a variable of type `any` or `interface`, Swag matches cases based on the underlying type of the variable. This behavior is akin to the `@kindof` intrinsic, which allows for dynamic type checking within a switch construct.
*/

#test
{
    let x: any = "value"

    // Switch statement based on the underlying type of 'x'.
    switch x // Implicitly checks the type of `x` using `@kindof`.
    {
    case string:
        break // Executes if `x` is of type string.
    default:
        @assert(false) // Assertion fails if `x` is not a string.
    }
}

/**
# Switch Statement with Type Guard and Variable Binding

You can declare a variable with the 'as' keyword after the case value to bind the matched value to a variable, which can then be used within the corresponding case block. Additionally, you can apply conditional checks directly within the case using the `where` clause to further refine the matching logic.

The switch statement implicitly checks the type of the variable `x` using a type guard (`@kindof`), allowing for more precise case matching based on the variable's runtime type.

Key Features:

1. Type Guarding: The switch statement evaluates the type of `x`, allowing cases to be matched based on its type.
2. Variable Binding: The matched value can be bound to a variable using the `var` keyword, making the value accessible within the case block.
3. Conditional Matching: The `where` clause enables additional conditions to be checked during case matching.
*/

/**
Example 1:

The first example checks if `x` is a string. If it is, the value is bound to the variable `str`.
The `@assert` function is then used to ensure that the value of `str` is equal to "value".
*/

#test
{
    let x: any = "value"

    // Switch statement based on the underlying type of 'x'.
    switch x // Implicitly checks the type of `x` using `@kindof`.
    {
    case string as str:
        @assert(str == "value")
        break

    default:
        @assert(false)
    }
}

/**
Example 2:

The second example showcases conditional matching using the `where` clause. The switch statement evaluates
whether `x` is a string and checks additional conditions specified in each case.
*/

#test
{
    let x: any = "value"

    switch x // Implicitly checks the type of `x` using `@kindof`.
    {
    case string as str where str == "value":
        @assert(str == "value") // Asserts that `str` is equal to "value"
        break
 // Matches if `x` is a string and equals "value"

    case string as str where str == "not_a_value":
        @assert(str == "not_a_value") // Asserts that `str` is equal to "not_a_value"
        break
 // Matches if `x` is a string and equals "not_a_value"

    default:
        @assert(false) // Asserts false if no case matches
    }
}

/**
# Switch Without an Expression
A `switch` statement in Swag can operate without an expression, behaving like a series of `if/else` statements. Each `case` is evaluated sequentially, and the first one that evaluates to `true` is executed, allowing for more complex conditional logic.
*/

#test
{
    let value  = 6
    let value1 = "true"

    // Switch statement without an expression, behaving like an if-else chain.
    switch
    {
    case value == 6 or value > 10:
        @assert(true) // Passes if `value` is 6 or greater than 10.
        fallthrough // Continues to the next case regardless.
    case value1 == "true":
        @assert(true) // Passes if `value1` is "true".
    default:
        @assert(false) // Fails if no cases match.
    }
}
