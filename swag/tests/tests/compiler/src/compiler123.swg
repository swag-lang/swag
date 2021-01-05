/*
#global testpass syntax;

attr toto() -> "func", "var"

#unittest error;
attr toto() -> "func",

#global testpass syntax;
#unittest error;
attr toto() -> "func", "func"

#global testpass syntax;
#unittest error;
attr toto() -> "var", "var"*/
