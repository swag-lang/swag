/*
    :LICENCE
    This code is based on the Go library (https://golang.org/src/unicode/tables.go)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global public
#global namespace Latin1

#[Swag.EnumFlags]
enum CharAttribute
{
    Zero        = 0
    Control
    Punctuation
    BinDigit
    HexDigit
    Digit
    Symbol
    Spacing
    Upper
    Lower
    Printable
    LowerUtf8
    UpperUtf8
    Graphical   = Printable | Spacing
    Letter      = Lower | Upper
}

const CharProperties: [256] CharAttribute = [
    Control, // '\x00'
    Control, // '\x01'
    Control, // '\x02'
    Control, // '\x03'
    Control, // '\x04'
    Control, // '\x05'
    Control, // '\x06'
    Control, // '\a'
    Control, // '\b'
    Spacing | Control, // '\t'
    Spacing | Control, // '\n'
    Spacing | Control, // '\v'
    Spacing | Control, // '\f'
    Spacing | Control, // '\r'
    Control, // '\x0e'
    Control, // '\x0f'
    Control, // '\x10'
    Control, // '\x11'
    Control, // '\x12'
    Control, // '\x13'
    Control, // '\x14'
    Control, // '\x15'
    Control, // '\x16'
    Control, // '\x17'
    Control, // '\x18'
    Control, // '\x19'
    Control, // '\x1a'
    Control, // '\x1b'
    Control, // '\x1c'
    Control, // '\x1d'
    Control, // '\x1e'
    Control, // '\x1f'
    Spacing | Printable, // ' '
    Punctuation | Printable, // '!'
    Punctuation | Printable, // '"'
    Punctuation | Printable, // '#'
    Symbol | Printable, // '$'
    Punctuation | Printable, // '%'
    Punctuation | Printable, // '&'
    Punctuation | Printable, // '\''
    Punctuation | Printable, // '('
    Punctuation | Printable, // ')'
    Punctuation | Printable, // '*'
    Symbol | Printable, // '+'
    Punctuation | Printable, // ','
    Punctuation | Printable, // '-'
    Punctuation | Printable, // '.'
    Punctuation | Printable, // '/'
    Digit | BinDigit | HexDigit | Printable, // '0'
    Digit | BinDigit | HexDigit | Printable, // '1'
    Digit | HexDigit | Printable, // '2'
    Digit | HexDigit | Printable, // '3'
    Digit | HexDigit | Printable, // '4'
    Digit | HexDigit | Printable, // '5'
    Digit | HexDigit | Printable, // '6'
    Digit | HexDigit | Printable, // '7'
    Digit | HexDigit | Printable, // '8'
    Digit | HexDigit | Printable, // '9'
    Punctuation | Printable, // ':'
    Punctuation | Printable, // ';'
    Symbol | Printable, // '<'
    Symbol | Printable, // '='
    Symbol | Printable, // '>'
    Punctuation | Printable, // '?'
    Punctuation | Printable, // '@'
    Upper | HexDigit | Printable, // 'A'
    Upper | HexDigit | Printable, // 'B'
    Upper | HexDigit | Printable, // 'C'
    Upper | HexDigit | Printable, // 'D'
    Upper | HexDigit | Printable, // 'E'
    Upper | HexDigit | Printable, // 'F'
    Upper | Printable, // 'G'
    Upper | Printable, // 'H'
    Upper | Printable, // 'I'
    Upper | Printable, // 'J'
    Upper | Printable, // 'K'
    Upper | Printable, // 'L'
    Upper | Printable, // 'M'
    Upper | Printable, // 'N'
    Upper | Printable, // 'O'
    Upper | Printable, // 'P'
    Upper | Printable, // 'Q'
    Upper | Printable, // 'R'
    Upper | Printable, // 'S'
    Upper | Printable, // 'T'
    Upper | Printable, // 'U'
    Upper | Printable, // 'V'
    Upper | Printable, // 'W'
    Upper | Printable, // 'X'
    Upper | Printable, // 'Y'
    Upper | Printable, // 'Z'
    Punctuation | Printable, // '['
    Punctuation | Printable, // '\\'
    Punctuation | Printable, // ']'
    Symbol | Printable, // '^'
    Punctuation | Printable, // '_'
    Symbol | Printable, // '`'
    Lower | HexDigit | Printable, // 'a'
    Lower | HexDigit | Printable, // 'b'
    Lower | HexDigit | Printable, // 'c'
    Lower | HexDigit | Printable, // 'd'
    Lower | HexDigit | Printable, // 'e'
    Lower | HexDigit | Printable, // 'f'
    Lower | Printable, // 'g'
    Lower | Printable, // 'h'
    Lower | Printable, // 'i'
    Lower | Printable, // 'j'
    Lower | Printable, // 'k'
    Lower | Printable, // 'l'
    Lower | Printable, // 'm'
    Lower | Printable, // 'n'
    Lower | Printable, // 'o'
    Lower | Printable, // 'p'
    Lower | Printable, // 'q'
    Lower | Printable, // 'r'
    Lower | Printable, // 's'
    Lower | Printable, // 't'
    Lower | Printable, // 'u'
    Lower | Printable, // 'v'
    Lower | Printable, // 'w'
    Lower | Printable, // 'x'
    Lower | Printable, // 'y'
    Lower | Printable, // 'z'
    Punctuation | Printable, // '{'
    Symbol | Printable, // '|'
    Punctuation | Printable, // '}'
    Symbol | Printable, // '~'
    Control, // '\u007f'
    Control | UpperUtf8, // '\u0080'
    Control | UpperUtf8, // '\u0081'
    Control | UpperUtf8, // '\u0082'
    Control | UpperUtf8, // '\u0083'
    Control | UpperUtf8, // '\u0084'
    Spacing | UpperUtf8, // '\u0085'
    Control | UpperUtf8, // '\u0086'
    Control | UpperUtf8, // '\u0087'
    Control | UpperUtf8, // '\u0088'
    Control | UpperUtf8, // '\u0089'
    Control | UpperUtf8, // '\u008a'
    Control | UpperUtf8, // '\u008b'
    Control | UpperUtf8, // '\u008c'
    Control | UpperUtf8, // '\u008d'
    Control | UpperUtf8, // '\u008e'
    Control | UpperUtf8, // '\u008f'
    Control | UpperUtf8, // '\u0090'
    Control | UpperUtf8, // '\u0091'
    Control | UpperUtf8, // '\u0092'
    Control | UpperUtf8, // '\u0093'
    Control | UpperUtf8, // '\u0094'
    Control | UpperUtf8, // '\u0095'
    Control | UpperUtf8, // '\u0096'
    Control, // '\u0097'
    Control | UpperUtf8, // '\u0098'
    Control | UpperUtf8, // '\u0099'
    Control | UpperUtf8, // '\u009a'
    Control | UpperUtf8, // '\u009b'
    Control | UpperUtf8, // '\u009c'
    Control | UpperUtf8, // '\u009d'
    Control | UpperUtf8, // '\u009e'
    Control, // '\u009f'
    Spacing | LowerUtf8, // '\u00a0'
    Punctuation | Printable | LowerUtf8, // '¡'
    Symbol | Printable | LowerUtf8, // '¢'
    Symbol | Printable | LowerUtf8, // '£'
    Symbol | Printable | LowerUtf8, // '¤'
    Symbol | Printable | LowerUtf8, // '¥'
    Symbol | Printable | LowerUtf8, // '¦'
    Punctuation | Printable | LowerUtf8, // '§'
    Symbol | Printable | LowerUtf8, // '¨'
    Symbol | Printable | LowerUtf8, // '©'
    Letter | Printable | LowerUtf8, // 'ª'
    Punctuation | Printable | LowerUtf8, // '«'
    Symbol | Printable | LowerUtf8, // '¬'
    LowerUtf8, // '\u00ad'
    Symbol | Printable | LowerUtf8, // '®'
    Symbol | Printable | LowerUtf8, // '¯'
    Symbol | Printable | LowerUtf8, // '°'
    Symbol | Printable | LowerUtf8, // '±'
    Digit | Printable | LowerUtf8, // '²'
    Digit | Printable | LowerUtf8, // '³'
    Symbol | Printable | LowerUtf8, // '´'
    Lower | Printable | LowerUtf8, // 'µ'
    Punctuation | Printable | LowerUtf8, // '¶'
    Punctuation | Printable, // '·'
    Symbol | Printable | LowerUtf8, // '¸'
    Digit | Printable | LowerUtf8, // '¹'
    Letter | Printable | LowerUtf8, // 'º'
    Punctuation | Printable | LowerUtf8, // '»'
    Digit | Printable | LowerUtf8, // '¼'
    Digit | Printable | LowerUtf8, // '½'
    Digit | Printable | LowerUtf8, // '¾'
    Punctuation | Printable, // '¿'
    Upper | Printable, // 'À'
    Upper | Printable, // 'Á'
    Upper | Printable, // 'Â'
    Upper | Printable, // 'Ã'
    Upper | Printable, // 'Ä'
    Upper | Printable, // 'Å'
    Upper | Printable, // 'Æ'
    Upper | Printable, // 'Ç'
    Upper | Printable, // 'È'
    Upper | Printable, // 'É'
    Upper | Printable, // 'Ê'
    Upper | Printable, // 'Ë'
    Upper | Printable, // 'Ì'
    Upper | Printable, // 'Í'
    Upper | Printable, // 'Î'
    Upper | Printable, // 'Ï'
    Upper | Printable, // 'Ð'
    Upper | Printable, // 'Ñ'
    Upper | Printable, // 'Ò'
    Upper | Printable, // 'Ó'
    Upper | Printable, // 'Ô'
    Upper | Printable, // 'Õ'
    Upper | Printable, // 'Ö'
    Symbol | Printable, // '×'
    Upper | Printable, // 'Ø'
    Upper | Printable, // 'Ù'
    Upper | Printable, // 'Ú'
    Upper | Printable, // 'Û'
    Upper | Printable, // 'Ü'
    Upper | Printable, // 'Ý'
    Upper | Printable, // 'Þ'
    Lower | Printable, // 'ß'
    Lower | Printable, // 'à'
    Lower | Printable, // 'á'
    Lower | Printable, // 'â'
    Lower | Printable, // 'ã'
    Lower | Printable, // 'ä'
    Lower | Printable, // 'å'
    Lower | Printable, // 'æ'
    Lower | Printable, // 'ç'
    Lower | Printable, // 'è'
    Lower | Printable, // 'é'
    Lower | Printable, // 'ê'
    Lower | Printable, // 'ë'
    Lower | Printable, // 'ì'
    Lower | Printable, // 'í'
    Lower | Printable, // 'î'
    Lower | Printable, // 'ï'
    Lower | Printable, // 'ð'
    Lower | Printable, // 'ñ'
    Lower | Printable, // 'ò'
    Lower | Printable, // 'ó'
    Lower | Printable, // 'ô'
    Lower | Printable, // 'õ'
    Lower | Printable, // 'ö'
    Symbol | Printable, // '÷'
    Lower | Printable, // 'ø'
    Lower | Printable, // 'ù'
    Lower | Printable, // 'ú'
    Lower | Printable, // 'û'
    Lower | Printable, // 'ü'
    Lower | Printable, // 'ý'
    Lower | Printable, // 'þ'
    Lower | Printable]
