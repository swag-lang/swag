#global skipfmt

/**
The `switch` statement in this language is similar to the one in C/C++, with a notable difference: there's no need for an explicit `break` statement at the end of each `case` block unless the `case` is empty. This prevents unintentional `fallthrough` behavior, where execution would continue from one `case` to the next without stopping.
*/
#test
{
    let value = 6

    // The switch statement here checks the value of 'value'.
    // Depending on the value, it executes the corresponding case.
    switch value
    {
    case 0:  
        @assert(false)  // If value is 0, this assertion fails (test fails).
    case 5:  
        @assert(false)  // If value is 5, this assertion fails (test fails).
    case 6:  
        @assert(true)   // If value is 6, this assertion passes (test succeeds).
    default: 
        @assert(false)  // If value doesn't match any case, this fails (test fails).
    }

    let ch = `A`'rune

    // This switch checks the value of 'ch'.
    switch ch
    {
    case `B`:
        @assert(false)  // If ch is 'B', this fails.
    case `A`: 
        break           // Exits the switch when 'ch' is 'A'.
    }
}

/**
You can assign multiple values to a single `case` statement, making it convenient to handle several potential matches with the same block of code.
*/
#test
{
    let value = 6

    // Here, case 2, 4, and 6 all trigger the same action.
    switch value
    {
    case 2, 4, 6:   
        @assert(true)   // If value is 2, 4, or 6, this assertion passes.
    default:        
        @assert(false)  // If value is anything else, this fails.
    }

    // The same concept, but with each value listed on its own line for clarity.
    switch value
    {
    case 2,
         4,
         6:
        @assert(true)   // Matches any of these values.
    default:
        @assert(false)  // Fails if no case matches.
    }
}

/**
The `switch` statement can be used with any type that supports the `==` operator. This includes not only numeric types but also strings, making it versatile for various comparisons.
*/
#test
{
    let value = "myString"

    // Switch works with strings, too.
    switch value
    {
    case "myString":    
        @assert(true)   // If value is "myString", this passes.
    case "otherString": 
        @assert(false)  // If value is "otherString", this fails.
    default:            
        @assert(false)  // If value doesn't match any case, this fails.
    }
}

/**
If you need behavior similar to C/C++ where one `case` can intentionally "fall through" to the next, you can use the `fallthrough` keyword. This explicitly tells the compiler to continue execution in the next `case` block.
*/
#test
{
    let value = 6

    // Demonstrates fallthrough: execution will continue from case 6 to case 7.
    switch value
    {
    case 6:
        fallthrough     // Fall through to the next case, even though value is still 6.
    case 7:
        @assert(value == 6) // This assertion checks that value is 6, even in case 7.
    default:
        @assert(true)   // Default case runs if no other cases match; true assertion keeps the test passing.
    }
}

/**
The `break` statement can be used to exit a `case` block early. This is useful if you only want to execute part of a `case` and then skip the rest of it.
*/
#test
{
    let value = 6

    // This switch demonstrates the use of break to exit early.
    switch value
    {
    case 6:
        if value == 6:
            break       // Exits the switch here if the condition is met.
        @assert(false)  // This will never be reached if value is 6.
    default:
        @assert(false)  // This fails if value doesn't match any case.
    }
}

/**
A `case` cannot be left empty. If you need a `case` to do nothing, use the `break` statement explicitly.
*/
#test
{
    let value = 6

    // This switch demonstrates handling cases where no action is needed.
    switch value
    {
    case 5:     
        @assert(false)  // This fails if value is 5.
    case 6:     
        break           // No action taken; exits the switch cleanly.
    default:    
        @assert(false)  // Fails if value doesn't match any case.
    }
}

/**
A `case` expression doesn't need to be a constant. You can use variables or even expressions that evaluate at runtime.
*/
#test
{
    let test = 2
    let a = 0
    let b = 1

    // This switch demonstrates the flexibility of using variables and expressions.
    switch test
    {
    case a:     // Here, 'a' is a variable, not a constant.
        @assert(false)  // Fails if test is equal to a (which is 0).
    case b:     // 'b' is another variable.
        @assert(false)  // Fails if test is equal to b (which is 1).
    case b + 1: // This case uses an expression.
        @assert(true)   // Passes if test equals 2 (b + 1).
    }
}

/**
# The 'Swag.Complete' Attribute
The `Swag.Complete` attribute ensures that all possible cases in an enum are handled in the `switch` statement. If any enum value is not covered by a `case`, the compiler will raise an error, ensuring exhaustive handling of all enum variants.
*/
#test
{
    enum Color { Red, Green, Blue }
    let color = Color.Red

    #[Swag.Complete]
    switch color
    {
    case Color.Red:     
        break           // Handle the Red case.
    case Color.Green:   
        @assert(false)  // Fails if color is Green.
    case Color.Blue:    
        @assert(false)  // Fails if color is Blue.
    }
}

/**
# Specific Types
When switching on a variable of type `any` or `interface`, the `switch` statement matches based on the underlying type of the variable. This behavior is similar to calling the `@kindof` intrinsic, which retrieves the type of the variable.
*/
#test
{
    let x: any = "value"

    // This switch is based on the type of 'x'.
    switch x    // Implicitly performs @kindof(x) to determine the type of 'x'.
    {
    case string: 
        break           // Matches if 'x' is a string.
    default:     
        @assert(false)  // Fails if 'x' is not a string.
    }
}

/**
# Using Ranges
A `switch` statement can match against a **range of values**. This is particularly useful for handling multiple values that fall within a specific range without having to list each one individually.
*/
#test
{
    var success = false
    let x = 6

    // This switch uses ranges to match a group of values.
    switch x
    {
    case 0 to 5:  
        @assert(false)  // Fails if x is between 0 and 5, inclusive.
    case 6 to 15: 
        success = true  // Succeeds if x is between 6 and 15, inclusive.
    }

    @assert(success)   // Ensures that the correct case was matched.
}

/**
If ranges overlap, the first valid range that matches will be used, and subsequent ranges are ignored.
*/
#test
{
    var success = false
    let x = 6

    // Demonstrates overlapping ranges and their precedence.
    switch x
    {
    case 0 to 10:    
        success = true  // This case is matched first, and the subsequent case is ignored.
    case 5 until 15: 
        @assert(false)  // This is not reached because the first case matches.
    }

    @assert(success)   // Confirms that the first range was correctly applied.
}

/**
# The 'where' clause
You can add a `where` clause to a `case` in a `switch` statement. The `where` clause allows you to add an additional condition that must be satisfied for that `case` to execute. This provides a finer level of control within your `switch` statements, enabling more complex decision-making based on multiple variables.
*/
#test
{
    let x = 6
    let y = 10

    // In this switch, each case not only checks the value of 'x',
    // but also applies an additional condition using 'where'.
    switch x
    {
    case 6 where y == 9:    
        // This case checks if 'x' is 6 and 'y' is 9.
        // 'y' is 10 here, so this case is skipped.
        @assert(false)   // This will not be executed.
    case 6 where y == 10:   
        // This case checks if 'x' is 6 and 'y' is 10.
        // Both conditions are true, so this case is executed.
        @assert(true)    // This assertion will pass.
    default:
        // This case is a fallback if no other cases match.
        @assert(false)   // This will not be executed since the second case matches.
    }
}

/**
The `where` clause can also be applied to a `default` case, providing additional filtering even when no specific `case` matches. This is particularly useful when you want the `default` behavior to occur only under certain conditions.
*/
#test
{
    let x = 7
    let y = 10

    // This switch demonstrates how different 'where' clauses can lead to different outcomes.
    switch x
    {
    case 6 where y == 10: 
        // Checks if 'x' is 6 and 'y' is 10.
        // 'x' is 7 here, so this case is skipped.
        @assert(false)   // This will not be executed.
    case 7 where y == 9:  
        // Checks if 'x' is 7 and 'y' is 9.
        // 'y' is 10 here, so this case is also skipped.
        @assert(false)   // This will not be executed.
    default where y == 10: 
        // This default case includes a 'where' clause.
        // If 'x' didn't match any previous case and 'y' is 10, this case is executed.
        break            // This will execute since 'y' is 10.
    default:
        // A general fallback default case.
        @assert(false)   // This will not be executed since the previous default matched.
    }
}


/**
# Switch Without an Expression
If no expression is provided in the `switch` statement, it behaves like a series of `if/else` statements. Each `case` is evaluated in order, and the first one that evaluates to `true` is executed.
*/
#test
{
    let value = 6
    let value1 = "true"

    // Demonstrates a switch statement without an expression.
    // This works like an if-else chain.
    switch
    {
    case value == 6 or value == 7:
        @assert(true)   // Passes if value is 6 or 7.
        fallthrough     // Continues to the next case regardless.
    case value1 == "true":
        @assert(true)   // Passes if value1 is "true".
    default:
        @assert(false)  // Fails if no previous case was matched.
    }
}
