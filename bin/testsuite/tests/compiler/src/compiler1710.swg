#test
{
    var str = "string"
    var tuple = @{@dataof(str), 2}
    ptr := @kindof(tuple)
}