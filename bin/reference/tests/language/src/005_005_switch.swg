/**
# Introduction to 'switch' in Swag

The `switch` statement in Swag behaves similarly to C/C++, with one key difference:
Swag **does not require `break`** at the end of each case.
Unintentional fallthroughs are prevented by default, ensuring each `case` is isolated unless explicitly linked with `fallthrough`.
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

A `case` can match multiple values, simplifying logic when several values share the same behavior.
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

    // Alternatively, list each value on its own line for clarity.
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

`switch` supports any type that implements `==`, including strings and user-defined comparable types.
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

Use `fallthrough` to intentionally continue execution into the next case.
*/

#test
{
    let value = 6

    switch value
    {
    case 6:
        fallthrough
    case 7:
        @assert(value == 6)
    default:
        @assert(true)
    }
}

/**
# Exiting a 'case' Early with 'break'

Use `break` to exit a `case` early when a certain condition is met.
*/

#test
{
    let value = 6

    switch value
    {
    case 6:
        if value == 6 do
            break
        @assert(false)
    default:
        @assert(false)
    }
}

/**
# Handling Empty Cases with 'break'

A `case` cannot be left empty. Use `break` explicitly when no action is required.
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

`switch` cases can use variables and expressions, evaluated dynamically at runtime.
*/

#test
{
    let test = 2
    let a    = 0
    let b    = 1

    switch test
    {
    case a:
        @assert(false)
    case b:
        @assert(false)
    case b + 1:
        @assert(true)
    }
}

/**
# The 'Swag.Complete' Attribute

Annotate a `switch` with `#[Swag.Complete]` when matching enums to enforce exhaustive case handling.
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

Swag supports matching a **range of values** in `case` conditions for concise range-based logic.
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

When ranges overlap, the **first matching range** is executed; later overlaps are ignored.
*/

#test
{
    var success = false
    let x       = 6

    switch x
    {
    case 0 to 10:
        success = true
    case 5 until 15:
        @assert(false)
    }

    @assert(success)
}

/**
# Using the 'where' Clause in 'switch'

Add a `where` clause to a `case` to refine matching conditions based on additional logic.
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

A `where` clause can also modify the `default` case for conditional fallbacks.
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

When switching on `any` or `interface` types, cases match based on the **underlying runtime type**.
*/

#test
{
    let x: any = "value"

    switch x
    {
    case string:
        break
    default:
        @assert(false)
    }
}

/**
# Switch Statement with Type Guard and Variable Binding

You can bind a matched type or value to a variable using `as`, allowing direct access inside the `case` block.
You may also use `where` for conditional refinement.
*/

/**
Example 1: Simple Type Binding
*/

#test
{
    let x: any = "value"

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
Example 2: Type Binding with 'where' Clause
*/

#test
{
    let x: any = "value"

    switch x
    {
    case string as str where str == "value":
        @assert(str == "value")
        break
    case string as str where str == "not_a_value":
        @assert(str == "not_a_value")
        break
    default:
        @assert(false)
    }
}

/**
# Switch Without an Expression

A `switch` without an expression behaves like an `if`/`elif` chain.
Each `case` is evaluated in order, executing the first one that evaluates to `true`.
*/

#test
{
    let value  = 6
    let value1 = "true"

    switch
    {
    case value == 6 or value > 10:
        @assert(true)
        fallthrough
    case value1 == "true":
        @assert(true)
    default:
        @assert(false)
    }
}
