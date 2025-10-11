func(T) largest(list: [..] T)->&T
{
    var largest = #ref &list[0]
    foreach item in list
    {
        if item > largest do
            largest = item
    }

    return largest
}

#test
{
    var number_list = [34, 50, 25, 100, 65]

    let result = largest(number_list[ to ])
    @assert(result == 100)

    var char_list = ['大''rune, "m", "a", "q"]

    let result1 = largest(char_list[ to ])
    @assert(result1 == '大''rune)
}
