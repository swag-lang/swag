func(T) toto(value: const [..] T)
{

}

#test
{
    toto'u8(cast(const [..] u8) "string")
}