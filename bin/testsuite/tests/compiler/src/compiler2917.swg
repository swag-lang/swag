func(T) swap(x, y: *T) {}

enum B { R, G, B }

#test
{
    var x, y = B.R
    swap(&x, &y)
}
