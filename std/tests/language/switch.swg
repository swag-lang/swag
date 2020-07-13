#test
{
    // 'switch' works like in C++, except that no 'break' is necessary
    {
        value := 6
        switch value
        {
            case 0:  @assert(false)
            case 5:  @assert(false)
            case 6:  @assert(true)
            default: @assert(false)
        }

        ch := "A"'char
        switch ch
        {
            case "B"'char: @assert(false)
            case "A"'char: break
        }
    }

    // You can put multiple value on the same 'case'
    {
        value := 6
        switch value
        {
            case 2, 4, 6:   @assert(true)
            default:        @assert(false)
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

    // If you want to pass from one case to another like in c++, use 'fallthrough'
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
}