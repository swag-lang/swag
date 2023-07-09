#global skip // remove typeset from language the 21 july 2021
typeset Foo
{
    a{string}
    b{isEnabled: bool}
}

#test
{
    var sth = Foo.a{"hello"}
    var oth = Foo.b{isEnabled: false}
}