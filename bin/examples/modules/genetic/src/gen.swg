using Core

struct Generation
{
    all: Array'Individual
}

struct Individual
{
    genes:  Array'u8
    score:  f32
}