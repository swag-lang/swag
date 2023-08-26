/**
`visit` is used to visit all the elements of a collection.
*/
#test
{
    // Here we visit every bytes of the string.
    // At each iteration, the byte will be stored in the variable 'value'
    visit value: "ABC"
    {
        // '@index' is also available. It stores the loop index.
        let a = @index
        switch a
        {
        case 0: @assert(value == `A`)
        case 1: @assert(value == `B`)
        case 2: @assert(value == `C`)
        }
    }
}

/**
You can name both the **value** and the loop **index**, in that order.
*/
#test
{
    visit value, index: "ABC"
    {
        let a = index
        switch a
        {
        case 0: @assert(value == `A`)
        case 1: @assert(value == `B`)
        case 2: @assert(value == `C`)
        }
    }
}

/**
Both names are optional. In that case, you can use `@alias0` and `@alias1`. `@alias0` for the value, and `@alias1` for the index.
*/
#test
{
    visit "ABC"
    {
        let a = @alias1    // This is the index
        @assert(a == @index)
        switch a
        {
        case 0: @assert(@alias0 == `A`)
        case 1: @assert(@alias0 == `B`)
        case 2: @assert(@alias0 == `C`)
        }
    }
}

/**
You can visit in reverse order by adding the 'back' modifier.
*/
#test
{
    // Here we visit every bytes of the string.
    // At each iteration, the byte will be stored in the variable 'value'
    var cpt = 0
    visit,back value: "ABC"
    {
        // '@index' is also available. It stores the loop index.
        switch cpt
        {
        case 0: @assert(value == `C`); @assert(@index == 2)
        case 1: @assert(value == `B`); @assert(@index == 1)
        case 2: @assert(value == `A`); @assert(@index == 0)
        }

        cpt += 1
    }
}

/**
You can visit arrays or slices.
*/
#test
{
    var array = [10, 20, 30]

    var result = 0
    visit it: array
        result += it

    @assert(result == 10+20+30)
}

/**
Works also for multi dimensional arrays.
*/
#test
{
    var array: [2, 2] s32 = [[10, 20], [30, 40]]

    var result = 0
    visit it: array
        result += it

    @assert(result == 10+20+30+40)
}

/**
You can visit with a pointer to the value, and not the value itself, by adding `&` before the value name.
*/
#test
{
    var array: [2, 2] s32 = [[1, 2], [3, 4]]

    var result = 0
    visit &it: array
    {
        result += dref it
        dref it = 555
    }

    @assert(result == 1+2+3+4)

    @assert(array[0, 0] == 555)
    @assert(array[0, 1] == 555)
    @assert(array[1, 0] == 555)
    @assert(array[1, 1] == 555)
}
