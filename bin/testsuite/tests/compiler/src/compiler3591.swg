#test
{
    let a = 666
    var cb = closure|a|() {
        var cb1 = closure|a|() {
            a = 0
        }
    }
}