using Core

//#[Swag.PrintBc]
#test
{
    let res: String = "A假C"
    let arr  = res.toRuneArray()
    @assert(arr.count == 3)
    @assert(arr[0] == 'A')
    @assert(arr[1] == '假')
    @assert(arr[2] == 'C')
}
