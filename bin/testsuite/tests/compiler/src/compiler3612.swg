func toto() {}

#test
{
    var t: [2] func()
    t[0] = &toto
    var x = t[0]
    x()
}