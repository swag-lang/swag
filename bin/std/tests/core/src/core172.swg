using Core

#test
{
    {
        let str: String = "this is a string"
        let res  = str[0 to 3]
        @assert(res == "this")
        let res1 = str[10 to str.length - 1]
        @assert(res1 == "string")
    }

    {
        let arr: Array'u32 = [1, 2, 3, 4, 5, 6]
        let res  = arr[3 to 5]
        @assert(res[0] == 4)
        @assert(res[1] == 5)
        @assert(res[2] == 6)
        @assert(@countof(res) == 3)
    }
}
