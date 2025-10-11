const MyString: string = #run
{
    var str: [3] u8
    str[0] = 'a'
    str[1] = 'b'
    str[2] = str[1] + 1
    return cast(string) str
}

#assert(MyString == "abc")
