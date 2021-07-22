#test
{
    // 'switch' works like in C, except that no 'break' is necessary (except if the 'case' is empty)
    {
        value := 6
        switch value
        {
        case 0:  @assert(false)
        case 5:  @assert(false)
        case 6:  @assert(true)
        default: @assert(false)
        }

        ch := "A"'rune
        switch ch
        {
        case "B"'rune: @assert(false)
        case "A"'rune: break
        }
    }

    // You can put multiple values on the same 'case'
    {
        value := 6
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

    // switch works with every types that accept the '==' operator
    {
        value := "myString"
        switch value
        {
        case "myString":    @assert(true)
        case "otherString": @assert(false)
        default:            @assert(false)
        }
    }

    // If you want to pass from one case to another like in C, use 'fallthrough'
    {
        value := 6
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

    // 'break' can be used to exit the current 'case' statement
    {
        value := 6
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

    // A 'case' statement cannot be empty. Use 'break' if you want to do nothing
    {
        value := 6
        switch value
        {
        case 5:     @assert(false)
        case 6:     break
        default:    @assert(false)
        }
    }

    // switch can be marked with 'Swag.Complete' to force all cases to be covered
    {
        enum Color { Red; Green; Blue; }
        color := Color.Red

        #[Swag.Complete]
        switch color
        {
        case Color.Red:     break
        case Color.Green:   @assert(false)
        case Color.Blue:    @assert(false)
        }
    }

    // If the switch expression is omitted, then it will behave like a serie of if/else,
    // resolved in order
    {
        value := 6
        value1 := "true"
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

    // When used on an 'any' variable, switch is done on the
    // underlying variable type
    {
        var x: any = "value"
        switch x
        {
        case string: break
        default:     @assert(false)
        }
    }
}