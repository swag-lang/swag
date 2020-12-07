#import "std"
using std

#test
{
    var arr: Array'String
    arr.addAny("0")
    arr.addAny("1")
    arr.addAny("2")
    arr.addAny("3")
    sort.insertion(@mkslice(arr.buffer, arr.count), @(x, y) => y <=> x)
    @assert(arr[0] == "3")
    @assert(arr[1] == "2")
    @assert(arr[2] == "1")
    @assert(arr[3] == "0")
    //visit *it: arr
    //    console.write("%", :it)
}