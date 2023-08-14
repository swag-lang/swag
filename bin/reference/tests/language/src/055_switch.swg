#test
{
    // `switch` works like in C/C++, except that no `break` is necessary (except if the `case` is empty). That means that there's no automatic `fallthrough` from one case to another.
    {
        let value = 6
        switch value
        {
        case 0:  @assert(false)
        case 5:  @assert(false)
        case 6:  @assert(true)
        default: @assert(false)
        }

        let ch = "A"'rune
        switch ch
        {
        case "B"'rune: @assert(false)
        case "A"'rune: break
        }
    }

    // You can put multiple values on the same `case`.
    {
        let value = 6
        switch value
        {
        case 2, 4, 6:   @assert(true)
        default:        @assert(false)
        }

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

    // `switch` works with every types that accept the `==` operator. So you can switch on strings for example.
    {
        let value = "myString"
        switch value
        {
        case "myString":    @assert(true)
        case "otherString": @assert(false)
        default:            @assert(false)
        }
    }

    // If you want to pass from one case to another like in C/C++, use `fallthrough`.
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

    // `break` can be used to exit the current `case` statement.
    {
        let value = 6
        switch value
        {
        case 6:
            if value == 6
                break
            @assert(false)
        default:
            @assert(false)
        }
    }

    // A `case` statement cannot be empty. Use `break` if you want to do nothing.
    {
        let value = 6
        switch value
        {
        case 5:     @assert(false)
        case 6:     break
        default:    @assert(false)
        }
    }

    // `switch` can be marked with `Swag.Complete` to force all the cases to be covered. If one or more values are missing, an error will be raised by the compiler.
    {
        enum Color { Red, Green, Blue }
        let color = Color.Red

        #[Swag.Complete]
        switch color
        {
        case Color.Red:     break
        case Color.Green:   @assert(false)
        case Color.Blue:    @assert(false)
        }
    }

    // If the switch expression is omitted, then it will behave like a serie of if/else, resolved in order.
    {
        let value = 6
        let value1 = "true"

        switch
        {
        case value == 6 or value == 7:
            @assert(true)
            fallthrough
        case value1 == "true":
            @assert(true)
        default:
            @assert(false)
        }
    }

    // When used on an `any` variable, switch is done on the underlying variable type.
    {
        let x: any = "value"

        switch x
        {
        case string: break
        default:     @assert(false)
        }
    }

    // A `switch` can also be used with a (constant) range of values.
    {
        var success = false
        let x = 6
        switch x
        {
        case 0..5:  @assert(false)
        case 6..15: success = true
        }

        @assert(success)
    }

    // If they overlap, the first valid range will be used.
    {
        var success = false
        let x = 6
        switch x
        {
        case 0..10:  success = true
        case 5..<15: @assert(false)
        }

        @assert(success)
    }

    // A `case` expression doesn't need to be constant.
    {
        let test = 2
        let a = 0
        let b = 1

        switch test
        {
        case a:     // Test with a variable
            @assert(false)
        case b:     // Test with a variable
            @assert(false)
        case b + 1: // Test with an expression
            @assert(true)
        }
    }
}