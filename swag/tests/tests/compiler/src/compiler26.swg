
#global testpass lexer;

"A"'char
"0"'char "\n"'char"\r"'char
"\x01"'char
"\u1234"'char
"\U12345678"'char

#unittest error;
"\xFFF"'char
