#test
{
    a := 666
    cb := closure|a|() {
        cb1 := closure|a|() {
            a = 0
        }
    }
}