typeset Foo
{
    a{string}
    b{isEnabled: bool}
}

#test
{
    sth := Foo.a{"hello"}
    oth := Foo.b{isEnabled: false}
}