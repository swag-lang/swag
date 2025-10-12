/**
# Introduction to 'switch' in Swag
The 'switch' statement in Swag operates similarly to those in C/C++, with a key distinction: Swag does not require an explicit 'break' statement at the end of each 'case' block. Instead, it prevents unintentional 'fallthrough' behavior by design, except when the 'case' is empty, where a 'break' statement is necessary. This ensures that each 'case' is independent unless explicitly designed otherwise, reducing the risk of errors in control flow.
*/

#test
{
    let value = 6

    switch value
    {
    case 0:
        @assert(false)
    case 5:
        @assert(false)
    case 6:
        @assert(true)
    default:
        @assert(false)
    }

    let ch = 'A''rune

    switch ch
    {
    case 'B':
        @assert(false)
    case 'A':
        break
    }
}

/**
# Multiple Values in a 'case'
Swag allows you to assign multiple values to a single 'case' statement, simplifying the code when the same block of code should execute for several potential matches. This feature enhances the readability and maintainability of your switch statements.
*/

#test
{
    let value = 6

    switch value
    {
    case 2, 4, 6:
        @assert(true)
    default:
        @assert(false)
    }

    // Listing each value on its own line for enhanced readability.
    switch value
    {
    case 2,
         4,
         6:
        @assert(true)
    default:
        @assert(false)
    }
}

/**
# Using 'switch' with Various Types
The 'switch' statement in Swag is versatile, supporting any type that implements the '==' operator. This flexibility extends beyond numeric types to include strings and other comparable types, making it a powerful tool for various decision-making scenarios.
*/

#test
{
    let value = "myString"

    switch value
    {
    case "myString":
        @assert(true)
    case "otherString":
        @assert(false)
    default:
        @assert(false)
    }
}

/**
# Intentional Fallthrough with 'fallthrough'
Swag allows for intentional 'fallthrough' behavior, similar to C/C++, using the 'fallthrough' keyword. This feature provides the option to continue execution from one 'case' block to the next, which can be useful in scenarios where multiple cases share common logic.
*/

#test
{
    let value = 6

    switch value
    {
    case 6:
        fallthrough // Explicitly continues to the next 'case'.
    case 7:
        @assert(value == 6)
    default:
        @assert(true)
    }
}

/**
# Exiting a 'case' Early with 'break'
The 'break' statement in Swag allows for early exit from a 'case' block. This is particularly useful when only a portion of the block should be executed under specific conditions, enhancing control flow within your 'switch' statements.
*/

#test
{
    let value = 6

    switch value
    {
    case 6:
        if value == 6 do
            break
        @assert(false) // This line is never reached if 'value' is 6.
    default:
        @assert(false)
    }
}

/**
# Handling Empty Cases with 'break'
In Swag, a 'case' statement cannot be left empty. If no action is required for a particular 'case', the 'break' statement must be used explicitly to avoid compilation errors, ensuring clarity in control flow.
*/

#test
{
    let value = 6

    switch value
    {
    case 5:
        @assert(false)
    case 6:
        break
    default:
        @assert(false)
    }
}

/**
# Variable and Expression Cases
Swag's 'switch' statement offers the flexibility to use variables and expressions in 'case' conditions. This allows for dynamic evaluation based on runtime values, further extending the utility of the 'switch' statement.
*/

#test
{
    let test = 2
    let a    = 0
    let b    = 1

    switch test
    {
    // 'a' is a variable here, not a constant.
    case a:
        @assert(false)
    // 'b' is another variable.
    case b:
        @assert(false)
    case b + 1:
        @assert(true)
    }
}

/**
# The 'Swag.Complete' Attribute
The 'Swag.Complete' attribute ensures exhaustive handling of all possible cases in an enum within a 'switch' statement. If any enum value is not explicitly handled, the compiler will raise an error, enforcing robust and complete logic coverage.
*/

#test
{
    enum Color { Red, Green, Blue }
    let color = Color.Red

    #[Swag.Complete]
    switch color
    {
    case Color.Red:
        break
    case Color.Green:
        @assert(false)
    case Color.Blue:
        @assert(false)
    }
}

/**
# Matching Ranges in a 'switch' Statement
Swag supports matching a **range of values** in a 'switch' statement, allowing you to group and handle multiple values that fall within a specific range efficiently. This is useful for concise and clear range-based logic within your code.
*/

#test
{
    var success = false
    let x       = 6

    switch x
    {
    case 0 to 5:
        @assert(false)
    case 6 to 15:
        success = true
    }

    @assert(success)
}

/**
# Overlapping Ranges
In Swag, if ranges within a 'switch' statement overlap, the first valid range that matches will be executed, while subsequent overlapping ranges are ignored. This order of precedence must be carefully considered in your logic.
*/

#test
{
    var success = false
    let x       = 6

    switch x
    {
    case 0 to 10:
        success = true // This case matches first; subsequent cases are ignored.
    case 5 until 15:
        @assert(false) // This is not reached because the first range matches.
    }

    @assert(success)
}

/**
# Using the 'where' Clause in 'switch'
The 'where' clause in Swag allows you to add additional conditions to a 'case' in a 'switch' statement. This provides finer control over the logic, enabling complex decision-making scenarios where multiple variables influence the outcome.
*/

#test
{
    let x = 6
    let y = 10

    switch x
    {
    case 6 where y == 9:
        @assert(false)
    case 6 where y == 10:
        @assert(true)
    default:
        @assert(false)
    }
}

/**
# Using 'where' with 'default'
The 'where' clause can also be applied to a 'default' case, providing conditional logic even when no specific 'case' matches. This allows you to handle default scenarios with more precision, based on additional criteria.
*/

#test
{
    let x = 7
    let y = 10

    switch x
    {
    case 6 where y == 10:
        @assert(false)
    case 7 where y == 9:
        @assert(false)
    default where y == 10:
        break
    default:
        @assert(false)
    }
}

/**
# Switching on Type with 'any' or 'interface'
When using a 'switch' statement with a variable of type 'any' or 'interface', Swag matches cases based on the underlying type of the variable. This behavior is akin to the '@kindof' intrinsic, which allows for dynamic type checking within a switch construct.
*/

#test
{
    let x: any = "value"

    // Switch statement based on the underlying type of 'x'.
    switch x // Implicitly checks the type of 'x' using '@kindof'.
    {
    case string:
        break
    default:
        @assert(false)
    }
}

/**
# Switch Statement with Type Guard and Variable Binding

You can declare a variable with the 'as' keyword after the case value to bind the matched value to a variable, which can then be used within the corresponding case block. Additionally, you can apply conditional checks directly within the case using the 'where' clause to further refine the matching logic.

The switch statement implicitly checks the type of the variable 'x' using a type guard ('@kindof'), allowing for more precise case matching based on the variable's runtime type.

Key Features:

1. Type Guarding: The switch statement evaluates the type of 'x', allowing cases to be matched based on its type.
2. Variable Binding: The matched value can be bound to a variable using the 'var' keyword, making the value accessible within the case block.
3. Conditional Matching: The 'where' clause enables additional conditions to be checked during case matching.
*/

/**
Example 1:

The first example checks if 'x' is a string. If it is, the value is bound to the variable 'str'.
The '@assert' function is then used to ensure that the value of 'str' is equal to "value".
*/

#test
{
    let x: any = "value"

    // Switch statement based on the underlying type of 'x'.
    // Implicitly checks the type of 'x' using '@kindof'.
    switch x
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

The second example showcases conditional matching using the 'where' clause. The switch statement evaluates
whether 'x' is a string and checks additional conditions specified in each case.
*/

#test
{
    let x: any = "value"

    // Implicitly checks the type of 'x' using '@kindof'.
    switch x
    {
    // Matches if 'x' is a string and equals "value"
    case string as str where str == "value":
        @assert(str == "value")
        break

    // Matches if 'x' is a string and equals "not_a_value"
    case string as str where str == "not_a_value":
        @assert(str == "not_a_value")
        break

    default:
        @assert(false)
    }
}

/**
# Switch Without an Expression
A 'switch' statement in Swag can operate without an expression, behaving like a series of 'if/else' statements. Each 'case' is evaluated sequentially, and the first one that evaluates to 'true' is executed, allowing for more complex conditional logic.
*/

#test
{
    let value  = 6
    let value1 = "true"

    switch
    {
    case value == 6 or value > 10:
        @assert(true)
        fallthrough // Continues to the next case regardless.
    case value1 == "true":
        @assert(true)
    default:
        @assert(false)
    }
}
