// #assert is a static assert (at compile time)
#assert true
#assert(true, "message")

// #defined(SYMBOL) returns true, at compile time, if the given symbol exists
// in the current context
#assert !#defined(DOES_NOT_EXISTS)
#assert #defined(Global)
var Global = 0

// ...