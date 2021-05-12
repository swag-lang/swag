func(T) swap(x, y: *T)
{

}

enum B { R; G; B; }

#test
{
    x, y := B.R
    swap(&x, &y)
}