func(T) largest(list: [..] T)->ref T
{
    largest := ref &list[0]
    visit item: list
    {
        if item > largest
            largest = item
    }

    return largest
}

#test
{
    number_list := [34, 50, 25, 100, 65]

    result := largest(number_list[..])
    @assert(result == 100)

    char_list := ["大"'rune, "m", "a", "q"]

    result1 := largest(char_list[..])
    @assert(result1 == "大"'rune)
}