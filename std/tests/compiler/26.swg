#unittest pass lexer;

'A'
'0' '\n''\r'
'\x01'
'\u1234'
'\U12345678'

#unittest error;
'\xFFF'

#unittest error;
'00