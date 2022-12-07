/**
In Swag, there's no need for ';' to end a statement like in C/C++.
Most of the time a end of line is enough.
*/
#test
{
    var x, y = 0
    x += 1
    y += 1
    @assert(x == 1)
    @assert(y == x)
}

/**
But ';' is possible too
*/
#test
{
    var x, y = 0;
    x += 1;
    y += 1;
}

/**
This is usefull if you want to do multiple things on the same line.
*/
#test
{
    var x = 0; var y = 0
    x += 1; y += 1
    @assert(x == 1); @assert(y == 1)
}