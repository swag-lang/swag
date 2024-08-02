#global testerror "Err0694"

sum(nums: f64...) {
    @assert(@countof(nums) == 3)
}

/*func sum(nums: f64, ...) {
    @assert(@countof(nums) == 3)
}*/

/*func sum(nums: f64...) {
    @assert(@countof(nums) == 3)*/

/*func sum(nums: f64...) {
    @assert @countof(nums) == 3
}*/

/*func sum(nums: f64...) {
    @assert(nums.0 == 10)
}*/

/*func sum(nums: f64...) {
    @check(@countof(nums) == 3)
}*/

/*func sum(int nums...) {
    @assert(@countof(nums) == 3)
}*/

/*func sum nums: f64... {
    @assert(@countof(nums) == 3)
}*/

/*private Bot
{
    id: s32
    count: s32
}
*/

/*private struct Bot
{
    id
    count: s32
}*/

/*private struct Bot
{
    val: [s32 2] = Swag.S32.Max
}*/

/*protected struct Bot
{
    id: s32
    count: s32
}*/

/*private struct Bot
{
    123id: s32
    count: s32
}*/

/*
private struct Bot
{
    val: [2] s32 = { Swag.S32.Max }
}*/

/*#test
{
    a = Vector{1, 1}
    var b = Vector{2, 5}
    var c = a - b
    @assert(c.x == -1)
    @assert(c.y == -4)
}*/

/*#test
(
    var a = Vector{1, 1}
)*/

/*
#test
{
    struct Vector{a, b: s32;}
    var a = Vector{1, 1}
    var b = Vector{2, 5}
    var c = a -- b
    @assert(c.x == -1)
    @assert(c.y == -4)
}*