//#global skip

struct(T) A {}

struct(T) B
{
    using a:     A'T
    using b:     A'T
}
