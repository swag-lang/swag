#include "pch.h"
#include "ErrorIds.h"

#undef SWAG_ERROR
#define SWAG_ERROR(__enum) #__enum
const char* g_EI[] = {
#include "ErrorList.h"
};

Utf8 g_E[MAX_ERRORS];
#undef SWAG_ERROR
#define SWAG_ERROR(__n, __msg)               \
    {                                        \
        SWAG_ASSERT(g_E[(int) __n].empty()); \
        g_E[(int) __n] = __msg;              \
    }
void initErrors()
{
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    SWAG_ERROR(Fat0001, "[cmdline] [[%s]] needs [[true]] or [[false]] as argument ([[%s]] provided)");
    SWAG_ERROR(Fat0002, "[cmdline] [[%s]] needs a string argument");
    SWAG_ERROR(Fat0003, "[cmdline] [[%s]] needs an integer argument ([[%s]] provided)");
    SWAG_ERROR(Fat0004, "[cmdline] [[%s]] needs an integer argument");
    SWAG_ERROR(Fat0005, "[cmdline] [[%s]] requires [[%s]]");
    SWAG_ERROR(Fat0006, "[cmdline] [[--stack-size]] out of range (%s provided, range is [%s, %s])");
    SWAG_ERROR(Fat0007, "[cmdline] unknown argument [[%s]]");
    SWAG_ERROR(Fat0008, "[cmdline] unsupported [[--arch]] [[%s]] for SCBE backend");
    SWAG_ERROR(Fat0009, "[cmdline] unsupported [[--os]] [[%s]] for SCBE backend");
    SWAG_ERROR(Fat0010, "[fatal] cache directory [[%s]] not found");
    SWAG_ERROR(Fat0011, "[fatal] dependency module [[%s]] can't be found in that workspace");
    SWAG_ERROR(Fat0012, "[fatal] duplicated module name; more than one module with the name [[%s]] is present in the workspace (path is [[%s]])");
    SWAG_ERROR(Fat0013, "[fatal] expected workspace folder [[--workspace]]");
    SWAG_ERROR(Fat0014, "[fatal] failed due to compiler bootstrap issues");
    SWAG_ERROR(Fat0015, "[fatal] failed due to compiler runtime issues");
    SWAG_ERROR(Fat0016, "[fatal] failed to create cache directory [[%s]]");
    SWAG_ERROR(Fat0017, "[fatal] failed to create directory [[%s]]");
    SWAG_ERROR(Fat0018, "[fatal] failed to create file [[%s]]");
    SWAG_ERROR(Fat0019, "[fatal] failed to create public directory [[%s]]");
    SWAG_ERROR(Fat0020, "[fatal] failed to create script file [[%s]]");
    SWAG_ERROR(Fat0021, "[fatal] failed to create target directory [[%s]]");
    SWAG_ERROR(Fat0022, "[fatal] failed to delete directory [[%s]]");
    SWAG_ERROR(Fat0023, "[fatal] failed to delete file [[%s]]");
    SWAG_ERROR(Fat0024, "[fatal] invalid [[--tag:%s]] value (%s)");
    SWAG_ERROR(Fat0025, "[fatal] invalid script file extension; expected [[.swgs]] found [[%s]]");
    SWAG_ERROR(Fat0026, "[fatal] invalid [[Swag]] command [[%s]]");
    SWAG_ERROR(Fat0027, "[fatal] invalid workspace [[%s]]; missing [[modules/]] or [[tests/]] sub-folder");
    SWAG_ERROR(Fat0028, "[fatal] module [[%s]] already exists in [[/modules]]");
    SWAG_ERROR(Fat0029, "[fatal] module [[%s]] can't be found in that workspace");
    SWAG_ERROR(Fat0030, "[fatal] script file [[%s]] not found");
    SWAG_ERROR(Fat0031, "[fatal] script file missing (use [[--file:<filename>.swgs]])");
    SWAG_ERROR(Fat0032, "[fatal] unexpected compile tag value [[%s]] (use [[--tag:%s]])");
    SWAG_ERROR(Fat0033, "[fatal] unknown compile tag type [[%s]] (use [[--tag:%s]])");
    SWAG_ERROR(Fat0034, "[fatal] workspace folder [[%s]] already exists");
    SWAG_ERROR(Fat0035, "[fatal] workspace folder [[%s]] not found");
    SWAG_ERROR(Fat0036, "[fatal] unable to locate the windows sdk folder");

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    SWAG_ERROR(Saf0001, "safety failed [[any]] $ invalid dynamic cast from [[any]] to [[%s]]");
    SWAG_ERROR(Saf0002, "safety failed [[any]] $ invalid dynamic cast of a null value to [[%s]]");
    SWAG_ERROR(Saf0003, "safety failed [[bool]] $ invalid boolean value");
    SWAG_ERROR(Saf0004, "safety failed [[boundcheck]] $ index out of bounds");
    SWAG_ERROR(Saf0005, "safety failed [[boundcheck]] $ range error, lower > upper");
    SWAG_ERROR(Saf0006, "safety failed [[boundcheck]] $ slice error, lower > upper");
    SWAG_ERROR(Saf0007, "safety failed [[boundcheck]] $ slice error, upper out of bounds");
    SWAG_ERROR(Saf0008, "safety failed [[math]] $ [[@abs]] overflow (type is [[%s]])");
    SWAG_ERROR(Saf0009, "safety failed [[math]] $ [[@acos]] invalid argument (type is [[%s]])");
    SWAG_ERROR(Saf0010, "safety failed [[math]] $ [[@asin]] invalid argument (type is [[%s]])");
    SWAG_ERROR(Saf0011, "safety failed [[math]] $ [[@log]] invalid argument (type is [[%s]])");
    SWAG_ERROR(Saf0012, "safety failed [[math]] $ [[@log10]] invalid argument (type is [[%s]])");
    SWAG_ERROR(Saf0013, "safety failed [[math]] $ [[@log2]] invalid argument (type is [[%s]])");
    SWAG_ERROR(Saf0014, "safety failed [[math]] $ [[@sqrt]] invalid argument (type is [[%s]])");
    SWAG_ERROR(Saf0015, "safety failed [[math]] $ division by zero");
    SWAG_ERROR(Saf0016, "safety failed [[nan]] $ invalid float (NaN)");
    SWAG_ERROR(Saf0017, "safety failed [[null]] $ dereferencing a null pointer");
    SWAG_ERROR(Saf0018, "safety failed [[null]] $ trying to throw a non existing error");
    SWAG_ERROR(Saf0019, "safety failed [[overflow]] $ negative value [[%I64d]] on unsigned type [[%s]]");
    SWAG_ERROR(Saf0020, "safety failed [[overflow]] $ negative value [[%s]] on unsigned type [[%s]]");
    SWAG_ERROR(Saf0021, "safety failed [[overflow]] $ negative value [[%g]] on unsigned type [[%s]]");
    SWAG_ERROR(Saf0022, "safety failed [[overflow]] $ value [[%s]] ([[%I64u]] in decimal) does not fit in type [[%s]]");
    SWAG_ERROR(Saf0023, "safety failed [[overflow]] $ value [[%I64u]] does not fit in type [[%s]]");
    SWAG_ERROR(Saf0024, "safety failed [[overflow]] $ value [[%I64d]] does not fit in type [[%s]]");
    SWAG_ERROR(Saf0025, "safety failed [[overflow]] $ value [[%g]] does not fit in type [[%s]]");
    SWAG_ERROR(Saf0026, "safety failed [[overflow]] $ overflow in [[%s]] (type is [[%s]])");
    SWAG_ERROR(Saf0027, "safety failed [[overflow]] $ truncation from [[%s]] to [[%s]]");
    SWAG_ERROR(Saf0028, "safety failed [[switch]] $ unexpected switch value ([[Swag.Complete]])");

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    SWAG_ERROR(San0001, "[sanity] division by zero    $ possible division by zero");
    SWAG_ERROR(San0002, "[sanity] division by zero    $ possible division by zero of %s [[%s]]");
    SWAG_ERROR(San0003, "[sanity] frame escape        $ attempt to return a local or a temporary variable");
    SWAG_ERROR(San0004, "[sanity] frame escape        $ attempt to return of %s [[%s]]");
    SWAG_ERROR(San0005, "[sanity] null dereference    $ attempt to dereference a null pointer");
    SWAG_ERROR(San0006, "[sanity] null dereference    $ attempt to dereference null %s [[%s]]");
    SWAG_ERROR(San0007, "[sanity] stack overwrite     $ attempt to overwrite the stack memory at offset [[%lld]] (stack size is [[%lld]])");
    SWAG_ERROR(San0008, "[sanity] uninitialized usage $ attempt to use uninitialized %s [[%s]]");
    SWAG_ERROR(San0009, "[sanity] uninitialized usage $ attempt to use uninitialized stack memory");
    SWAG_ERROR(San0010, "[sanity] overflow            $ possible overflow in [[%s]] (type is [[%s]])");

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    SWAG_ERROR(Wrn0001, "%s");
    SWAG_ERROR(Wrn0002, "deprecated usage         $ %s [[%s]] is deprecated (attribute [[Swag.Deprecated]])");
    SWAG_ERROR(Wrn0003,
               "unchanged local variable $ the variable [[%s]] remains unchanged after its declaration $ if you don't intend to modify it, consider declaring it with [[let]] instead of [[var]] for immutability");
    SWAG_ERROR(Wrn0004,
               "unchanged local variable $ the variable [[%s]] is declared to its default value, and is never modified after that $ are you sure this variable is necessary?");
    SWAG_ERROR(Wrn0005, "unreachable code         $ this code is not reachable and will never be executed");
    SWAG_ERROR(Wrn0006, "unused %s                $ %s [[%s]] is not used");

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    SWAG_ERROR(Err0001, "%s");
    SWAG_ERROR(Err0002, "%s");
    SWAG_ERROR(Err0003, "%s");
    SWAG_ERROR(Err0004, "[[#global export]] already defined                $ only one [[#global export]] is allowed per file");
    SWAG_ERROR(Err0005, "[[#main]] already defined                         $ only one [[#main]] function is allowed per module");
    SWAG_ERROR(Err0006, "[[@mkany]] inconsistency                          $ the pointer type [[%s]] and the second argument [[%s]] are unrelated");
    SWAG_ERROR(Err0007, "[[default]] already defined                       $ only one [[default]] statement is allowed per [[switch]]");
    SWAG_ERROR(Err0008, "[[impl]] kind mismatch                            $ the implementation kind (%s) and the type of [[%s]] (%s) are not the same");
    SWAG_ERROR(Err0009, "[[switch]] value already defined                  $ the [[switch]] value [[%d]] has already been defined");
    SWAG_ERROR(Err0010, "[[switch]] value already defined                  $ the [[switch]] value [[%f]] has already been defined");
    SWAG_ERROR(Err0011, "[[switch]] value already defined                  $ the [[switch]] value [[%s]] has already been defined");
    SWAG_ERROR(Err0012, "access modifier already defined                   $ invalid use of a [[%s]] access modifier after [[%s]]");
    SWAG_ERROR(Err0013, "ambiguous [[using]]                               $ unexpected [[using]] on two variables with the same type ([[%s]])");
    SWAG_ERROR(Err0014, "ambiguous cast                                    $ there are multiple [[using]] fields of type [[%s]] in [[%s]]");
    SWAG_ERROR(Err0015, "ambiguous generic instantiation                   $ can't instantiate the generic %s [[%s]] with an unsized value");
    SWAG_ERROR(Err0016, "ambiguous symbol                                  $ ambiguous conversion of struct [[%s]] to interface [[%s]]");
    SWAG_ERROR(Err0017, "ambiguous symbol                                  $ ambiguous resolution of the %s [[%s]]");
    SWAG_ERROR(Err0018, "ambiguous symbol                                  $ ambiguous resolution of the enum value [[%s]]");
    SWAG_ERROR(Err0019, "ambiguous symbol                                  $ ambiguous resolution of the generic %s [[%s]]");
    SWAG_ERROR(Err0020, "ambiguous symbol                                  $ ambiguous resolution of the special function [[%s]]");
    SWAG_ERROR(Err0021, "ambiguous syntax                                  $ ambiguity arises between initializing [[%s]] and starting a new block");
    SWAG_ERROR(Err0022, "ambiguous syntax                                  $ ambiguous declaration within lambda type parameters");
    SWAG_ERROR(Err0023, "argument already defined                          $ the argument [[%s]] has already been defined");
    SWAG_ERROR(Err0024, "bytecode stack overflow                           $ the required size exceeds the limit [[%s]] given by [[--stack-size]]");
    SWAG_ERROR(Err0025, "can't resolve foreign call                        $ failed to load the module [[%s]] while resolving the foreign function [[%s]]: %s");
    SWAG_ERROR(Err0026, "capture size overflow                             $ the total requested size is [[%u]] bytes but the maximum authorized size is [[%u]]");
    SWAG_ERROR(Err0027, "compile-time evaluation failed                    $ can't transform the type [[%s]] to a constant array because [[opCount]] returns [[0]]");
    SWAG_ERROR(Err0028, "compile-time evaluation failed                    $ can't transform the type [[%s]] to a constant array because [[opSlice]] returns an empty slice");
    SWAG_ERROR(Err0029, "compile-time evaluation required                  $ the type [[%s]] can't be converted to constant expression");
    SWAG_ERROR(Err0030, "compile-time evaluation required                  $ [[#include]] requited its filename argument to be evaluated at compile-time");
    SWAG_ERROR(Err0031, "compile-time evaluation required                  $ [[%s]] can't be evaluated at compile-time");
    SWAG_ERROR(Err0032, "compile-time evaluation required                  $ a slice of type [[%s]] can't be converted to a compile-time value");
    SWAG_ERROR(Err0033, "compile-time evaluation required                  $ the [[%s]] argument can't be evaluated at compile-time");
    SWAG_ERROR(Err0034, "compile-time evaluation required                  $ the [[%s]] message can't be evaluated at compile-time");
    SWAG_ERROR(Err0035,
               "compile-time evaluation required                  $ the [[case]] expression can't be evaluated at compile-time $ the [[switch]] is marked as [[#[Swag.Complete]]], so the expression must be constant");
    SWAG_ERROR(Err0036, "compile-time evaluation required                  $ the array dimension can't be evaluated at compile-time");
    SWAG_ERROR(Err0037, "compile-time evaluation required                  $ the attribute parameter can't be evaluated at compile-time");
    SWAG_ERROR(Err0038, "compile-time evaluation required                  $ the expression can't be evaluated at compile-time");
    SWAG_ERROR(Err0039, "compile-time evaluation required                  $ the function parameter of type [[%s]] can't be evaluated at compile-time");
    SWAG_ERROR(Err0040,
               "compile-time evaluation required                  $ the initial value can't be evaluated at compile-time $ in order to make that kind of compile-time evaluation, [[opAffect]] should be [[#[Swag.ConstExpr]]]");
    SWAG_ERROR(Err0041, "compile-time evaluation required                  $ the initialization expression can't be evaluated at compile-time");
    SWAG_ERROR(Err0042,
               "compile-time evaluation required                  $ the operation on struct [[%s]] can't be done compile-time $ in order to make that kind of compile-time evaluation, the struct should be [[#[Swag.ConstExpr]]]");
    SWAG_ERROR(Err0043,
               "compile-time evaluation required                  $ the operation on the tuple can't be done compile-time $ in order to make that kind of compile-time evaluation, the tuple should be [[#[Swag.ConstExpr]]]");
    SWAG_ERROR(Err0044, "compile-time evaluation required                  $ the type constraint can't be evaluated at compile-time");
    SWAG_ERROR(Err0045, "compiler assertion failed                         $ the [[#assert]] expression was evaluated to false");
    SWAG_ERROR(Err0046, "compiler limit reached                            $ the size of the %s is too big (maximum size is [[0x%I64x]] bytes)");
    SWAG_ERROR(Err0047, "compiler limit reached                            $ the size of the data segment [[%s]] is too big (maximum size is [[0x%I64x]] bytes)");
    SWAG_ERROR(Err0048, "conflicting attributes                            $ the [[#[Swag.Compiler]]] and [[#[Swag.Tls]]] attributes are mutually exclusive");
    SWAG_ERROR(Err0049, "conflicting attributes                            $ the [[#[Swag.Inline]]] and [[#[Swag.NoInline]]] attributes are mutually exclusive");
    SWAG_ERROR(Err0050, "conflicting attributes                            $ the [[#[Swag.Macro]]] and [[#[Swag.Inline]]] attributes are mutually exclusive");
    SWAG_ERROR(Err0051, "conflicting attributes                            $ the [[#[Swag.Macro]]] and [[#[Swag.Mixin]]] attributes are mutually exclusive");
    SWAG_ERROR(Err0052, "conflicting attributes                            $ the [[#[Swag.Mixin]]] and [[#[Swag.Inline]]] attributes are mutually exclusive");
    SWAG_ERROR(Err0053, "conflicting cast modifiers                        $ the [[%s]] and [[%s]] cast modifiers are mutually exclusive");
    SWAG_ERROR(Err0054, "const mismatch                                    $ casting from an immutable type [[%s]] to a mutable one [[%s]] is not allowed");
    SWAG_ERROR(Err0055, "const mismatch                                    $ the UFCS argument should be mutable but is not (type is [[%s]])");
    SWAG_ERROR(Err0056, "const mismatch                                    $ the intrinsic [[%s]] requires a mutable pointer as a first argument, got [[%s]] instead");
    SWAG_ERROR(Err0057, "const mismatch                                    $ the intrinsic [[%s]] requires a mutable variable as a first argument, got [[%s]] instead");
    SWAG_ERROR(Err0058, "defer escape throw                                $ an error can't escape a [[defer]] or [[defer(err)]] block");
    SWAG_ERROR(Err0059, "dependency error                                  $ can't resolve the dependency to the module [[%s]] because of a version mismatched");
    SWAG_ERROR(Err0060, "dependency error                                  $ can't resolve the dependency to the module [[%s]] because version [[%s]] was not found at [[%s]]");
    SWAG_ERROR(Err0061, "dependency error                                  $ can't resolve the dependency to the module [[%s]] because version [[%s]] was not found");
    SWAG_ERROR(Err0062, "division by zero                                  $ this divider expression evaluates to 0");
    SWAG_ERROR(Err0063, "double initialization                             $ you should not initialize a struct with both the type syntax and an assignment");
    SWAG_ERROR(Err0064, "duplicated [[#import]] location                   $ the [[#import]] location has already been defined");
    SWAG_ERROR(Err0065, "duplicated [[#import]] version                    $ the [[#import]] version has already been defined");
    SWAG_ERROR(Err0066, "duplicated [[#import]]                            $ the [[#import]] location of the module [[%s]] is already defined as [[%s]]");
    SWAG_ERROR(Err0067, "duplicated [[#import]]                            $ the [[#import]] version of the module [[%s]] is already defined as [[%s]]");
    SWAG_ERROR(Err0068, "duplicated attribute                              $ the attribute [[%s]] is assigned twice, but [[Swag.AttrMulti]] is not present in the declaration");
    SWAG_ERROR(Err0069, "duplicated enum value                             $ the enum value [[%s]] is already defined with the same underlying value");
    SWAG_ERROR(Err0070, "duplicated instruction modifier                   $ an instruction modifier [[%s]] has already been defined");
    SWAG_ERROR(Err0071, "duplicated operator                               $ expected an expression, found another operator [[%s]] instead");
    SWAG_ERROR(Err0072,
               "empty [[#mixin]] replacement block                $ an empty [[mixin]] block is useless $ add some content to the [[#mixin]] block or consider removing it");
    SWAG_ERROR(Err0073, "empty [[case]] statement                          $ a [[case]] statement should not be empty");
    SWAG_ERROR(Err0074, "empty [[default]] statement                       $ a [[default]] statement should not be empty");
    SWAG_ERROR(Err0075, "empty [[interface]]                               $ the interface [[%s]] should contain at least one function declaration");
    SWAG_ERROR(Err0076, "empty [[switch]]                                  $ the body of the [[switch]] is empty $ consider adding some cases, or removing it");
    SWAG_ERROR(Err0077, "empty array literal                               $ an array literal should contain at least one value");
    SWAG_ERROR(Err0078, "empty compiler function body                      $ a %s requires a body; expected [[{]] but found [[;]] instead");
    SWAG_ERROR(Err0079, "missing namespace name                            $ expected the namespace name before [[%s]]");
    SWAG_ERROR(Err0080, "enum already defined                              $ the enum [[%s]] has already been defined");
    SWAG_ERROR(Err0081, "escaping stack frame                              $ you can't reference the %s [[%s]] because it's in a different stack frame");
    SWAG_ERROR(Err0082, "exception!                                        $ compile-time execution exception");
    SWAG_ERROR(Err0083, "expected [[%s]]                                   $ expected [[%s]] %s, found [[%s]] instead");
    SWAG_ERROR(Err0084, "failed [[#validif]]                               $ the [[#validif]] validation has failed for [[%s]]");
    SWAG_ERROR(Err0085, "failed [[%s]]                                     $ the function [[%s]] can't be used because of a failed [[%s]]");
    SWAG_ERROR(Err0086, "failed generic instantiation                      $ the variable creation failed due to the generic type [[%s]]");
    SWAG_ERROR(Err0087, "failed semantic                                   $ can't resolve this");
    SWAG_ERROR(Err0088, "failed type constraint                            $ the type constraint has failed for type [[%s]]");
    SWAG_ERROR(Err0089, "fetch error                                       $ can't fetch file [[%s]] for module dependency [[%s]]");
    SWAG_ERROR(Err0090, "file delete error                                 $ failed to delete file [[%s]]");
    SWAG_ERROR(Err0091, "file format error                                 $ the file format must be ASCII, UTF-8, or UTF-8-BOM");
    SWAG_ERROR(Err0092, "file not found                                    $ the documentation page [[%s]] can't be found");
    SWAG_ERROR(Err0093, "file not found                                    $ the file [[%s]] can't be found in the module folder [[%s]]");
    SWAG_ERROR(Err0094, "file open error                                   $ failed to access file [[%s]]");
    SWAG_ERROR(Err0095, "file open error                                   $ failed to open file [[%s]]");
    SWAG_ERROR(Err0096, "file open error                                   $ failed to open file [[%s]] for writing");
    SWAG_ERROR(Err0097, "file open error                                   $ failed to open the [[#include]] file [[%s]]");
    SWAG_ERROR(Err0098, "file read error                                   $ failed to read source file [[%s]]");
    SWAG_ERROR(Err0099, "file write error                                  $ failed to write to file [[%s]]");
    SWAG_ERROR(Err0100, "folder error                                      $ failed to create directory [[%s]]");
    SWAG_ERROR(Err0101, "forbidden [[@postcopy]]                           $ the intrinsic [[@postcopy]] can't be called on type [[%s]] because of [[#[Swag.NoCopy]]]");
    SWAG_ERROR(Err0102,
               "forbidden [[opDrop]]                              $ unexpected [[opDrop]] special function for [[%s]] because the struct is marked with [[#[Swag.ConstExpr]]]");
    SWAG_ERROR(Err0103,
               "forbidden [[opPostCopy]]                          $ unexpected [[opPostCopy]] special function for struct [[%s]] because the struct is marked with [[#[Swag.NoCopy]]]");
    SWAG_ERROR(Err0104, "forbidden assign                                  $ can't assign because the left expression is immutable");
    SWAG_ERROR(Err0105, "forbidden assign                                  $ can't assign from [[%s]] with type %s");
    SWAG_ERROR(Err0106, "forbidden assign                                  $ can't assign to [[%s]] because it's immutable");
    SWAG_ERROR(Err0107, "forbidden assign                                  $ can't assign to [[%s]] with type %s");
    SWAG_ERROR(Err0108, "forbidden call                                    $ a direct call to [[opDrop]] is forbidden $ consider using [[@drop]] instead]]");
    SWAG_ERROR(Err0109, "forbidden call                                    $ a direct call to [[opInit]] is forbidden $ consider using [[@init]] instead]]");
    SWAG_ERROR(Err0110, "forbidden call                                    $ a direct call to [[opPostCopy]] is forbidden $ consider using [[@postcopy]] instead]]");
    SWAG_ERROR(Err0111, "forbidden call                                    $ a direct call to [[opPostMove]] is forbidden $ consider using [[@postmove]] instead]]");
    SWAG_ERROR(Err0112,
               "forbidden call                                    $ the [[compileString]] function is not accessible in this context $ this compiler stage does not allow meta-programmation");
    SWAG_ERROR(Err0113, "forbidden copy                                    $ the copy of [[%s]] is disabled because the struct is marked with [[#[Swag.NoCopy]]]");
    SWAG_ERROR(Err0114, "generic symbol already defined                    $ the generic symbol [[%s]] has already been defined in a parent scope");
    SWAG_ERROR(Err0115, "generic type mismatch                             $ expected [[%s]] for the generic %s, got [[%s]] instead");
    SWAG_ERROR(Err0116, "identifier expected                               $ expected a function call after [[%s]], found [[%s]] instead");
    SWAG_ERROR(Err0117, "immutable global variable                         $ the global variable [[%s]] is immutable at compile-time because it's initialized to 0");
    SWAG_ERROR(Err0118,
               "incomplete [[case]] expression list               $ expected another expression after [[,]] and before [[:]] $ [[,]] indicates a list of [[case]] expressions; provide the next expression or consider removing the [[,]]");
    SWAG_ERROR(Err0119, "incomplete [[switch]]                             $ the value [[%s.%s]] is missing");
    SWAG_ERROR(Err0120,
               "incomplete alias list                             $ expected another alias name after [[,]] $ either provide an additional alias name or remove the trailing [[,]]");
    SWAG_ERROR(Err0121,
               "incomplete arguments list                         $ expected another call argument after [[,]] $ either provide an additional argument or remove the trailing [[,]]");
    SWAG_ERROR(Err0122,
               "incomplete array dimensions                       $ expected another dimension after [[,]] $ either provide an additional dimension or remove the trailing [[,]]");
    SWAG_ERROR(Err0123, "incomplete binary number format                   $ binary literals require at least one [[0]] or [[1]] digit");
    SWAG_ERROR(Err0124, "incomplete character literal                      $ expected a closing back-tick [[`]] before the end of the line");
    SWAG_ERROR(Err0125, "incomplete hexadecimal number format              $ hexadecimal literals require at least one valid digit ([[0-9]], [[A-F]], [[a-f]])");
    SWAG_ERROR(Err0126, "incomplete import version                         $ the [[#import]] version has build number [[%d]] but lacks a revision");
    SWAG_ERROR(Err0127, "incomplete import version                         $ the [[#import]] version has revision [[%d]] but lacks a major version");
    SWAG_ERROR(Err0128,
               "incomplete indexing                               $ expected another expression after [[,]] $ either provide an additional expression or remove the trailing [[,]]");
    SWAG_ERROR(Err0129, "incomplete interface                              $ some functions of the interface [[%s]] are not implemented for [[%s]]");
    SWAG_ERROR(Err0130,
               "incomplete interface function                     $ the interface member [[%s]] should have at least [[self]] as the first parameter $ consider declaring the interface function with [[mtd]] instead of [[func]]");
    SWAG_ERROR(Err0131,
               "incomplete parameters list                        $ expected another parameter after [[,]] $ either provide an additional parameter or remove the trailing [[,]]");
    SWAG_ERROR(Err0132,
               "incomplete string literal                         $ expected a closing quotation mark [[\"]] before the end of the line $ if you want a multi-line string, use the syntax [[\"\"\"string\"\"\"]]");
    SWAG_ERROR(Err0133,
               "incomplete tuple unpacking                        $ expected another variable name after [[,]] $ either provide an additional variable name or remove the trailing [[,]]");
    SWAG_ERROR(Err0134,
               "incomplete variables list                         $ expected another variable name after [[,]] $ either provide an additional variable name or remove the trailing [[,]]");
    SWAG_ERROR(Err0135,
               "incorrect import location                         $ the [[#import]] location has an incorrect format $ expecting [[location=\"mode@accesspath\"]] where mode is [[swag]] or [[disk]]");
    SWAG_ERROR(Err0136, "index out of bounds                               $ the given index [[%I64u]] exceeds the maximum value [[%I64u]]");
    SWAG_ERROR(Err0137,
               "infinite [[while]]                                $ the while condition is always [[true]], which creates an infinite loop $ if you intentionally want to create an infinite loop, consider using a loop without argument [[loop { ... }]]");
    SWAG_ERROR(Err0138, "invalid [[#alias]] number                         $ [[#alias]] variable names should end with a number, found [[%s]] instead");
    SWAG_ERROR(Err0139, "invalid [[#global]] instruction                   $ [[%s]] is not recognized as a valid [[#global]] instruction");
    SWAG_ERROR(Err0140, "invalid [[#import]] location                      $ expected a [[#import]] location path, found [[%]] instead");
    SWAG_ERROR(Err0141, "invalid [[#import]] location                      $ expected a [[#import]] version as a string, found [[%]] instead");
    SWAG_ERROR(Err0142, "invalid [[#mixin]] number                         $ [[#mixin]] variable names should end with a number, found [[%s]] instead");
    SWAG_ERROR(Err0143, "invalid [[#mixin]] replacement keyword            $ a [[#mixin]] block accepts only [[break]] and [[continue]] as replacements, found [[%s]] instead");
    SWAG_ERROR(Err0144, "invalid [[#scope]] identifier                     $ expected the [[#scope]] identifier, found [[%s]] instead");
    SWAG_ERROR(Err0145, "invalid [[#up]] count                             $ expected an integer literal for the [[#up]] count, found [[%s]] instead");
    SWAG_ERROR(Err0146, "invalid [[#up]] count                             $ the [[#up]] count can't be zero");
    SWAG_ERROR(Err0147, "invalid [[#up]] count                             $ the [[#up]] count should be an untyped integer in the range [1, 255], found [[%s]]");
    SWAG_ERROR(Err0148, "invalid [[#up]] move                              $ [[#up]] can't move [[%d]] scopes up");
    SWAG_ERROR(Err0149, "invalid [[@alignof]]                              $ can't compute the alignment of a generic expression");
    SWAG_ERROR(Err0150, "invalid [[@countof]] argument                     $ the intrinsic [[@countof]] requires a positive argument, got [[%I64d]]");
    SWAG_ERROR(Err0151, "invalid [[@countof]] argument                     $ the intrinsic [[@countof]] requires a positive argument, got [[%d]] instead");
    SWAG_ERROR(Err0152, "invalid [[@countof]] argument                     $ the intrinsic [[@countof]] requires an integer argument, got [[%s]] instead");
    SWAG_ERROR(Err0153, "invalid [[@cvaarg]] type                          $ a variadic argument of type [[%s]] has been promoted to type [[%s]] at the call site");
    SWAG_ERROR(Err0154, "invalid [[@sizeof]]                               $ can't compute the size of a generic expression");
    SWAG_ERROR(Err0155, "invalid [[break]] scope name                      $ expected a scope name or a end of line after [[break]], found [[%s]] instead");
    SWAG_ERROR(Err0156,
               "invalid [[code]] append                           $ the %s [[%s]] wants to interpret the next statement as a [[code]] parameter but this is not possible inside a [[%s]] expression");
    SWAG_ERROR(Err0157, "invalid [[defer]] mode                            $ [[%s]] is not a valid [[defer]] mode $ the valid defer modes are [[defer(err)]] and [[defer(noerr)]]");
    SWAG_ERROR(Err0158, "invalid [[discard]] call                          $ a function returning nothing can't be discarded");
    SWAG_ERROR(Err0159, "invalid [[discard]] call                          $ expected a discard-able call after [[discard]], found [[%s]] instead");
    SWAG_ERROR(Err0160, "invalid [[impl]]                                  $ expected a struct name after [[for]] but [[%s]] is %s");
    SWAG_ERROR(Err0161, "invalid [[impl]]                                  $ expected a struct name or an enum name after [[impl]] but [[%s]] is [[%s]]");
    SWAG_ERROR(Err0162, "invalid [[impl]]                                  $ expected an interface name after [[impl]] but [[%s]] is %s");
    SWAG_ERROR(Err0163, "invalid [[namealias]]                             $ can't alias a struct member");
    SWAG_ERROR(Err0164, "invalid [[opVisit]] name                          $ an [[opVisit]] variant should be a valid identifier, found [[%s]] instead");
    SWAG_ERROR(Err0165, "invalid [[orelse]] type                           $ the [[orelse]] operator does not accept the type [[%s]] as an argument");
    SWAG_ERROR(Err0166, "invalid [[orelse]] type                           $ the [[orelse]] operator does not accept a [[struct]] as an argument");
    SWAG_ERROR(Err0167, "invalid [[retval]]                                $ the [[retval]] special type is not allowed in a function that returns nothing");
    SWAG_ERROR(Err0168, "invalid [[switch]] block content                  $ expected [[case]] or [[default]], found [[%s]] instead");
    SWAG_ERROR(Err0169, "invalid [[switch]] type                           $ [[switch]] does not accept an expression of type [[%s]]");
    SWAG_ERROR(Err0170, "invalid [[visit]] name                            $ expected a [[visit]] specialized name, found [[%s]] instead");
    SWAG_ERROR(Err0171, "invalid [[visit]] type                            $ a tuple can't be visited");
    SWAG_ERROR(Err0172, "invalid [[with]]                                  $ [[with]] is invalid on an enum variable (type is [[%s]])");
    SWAG_ERROR(Err0173, "invalid [[with]]                                  $ [[with]] is invalid on type [[%s]] $ expected a namespace, an enum, a struct or a pointer to struct");
    SWAG_ERROR(Err0174, "invalid [[with]] expression                       $ expected a [[with]] valid expression");
    SWAG_ERROR(Err0175, "invalid access from runtime                       $ the compile-time %s [[%s]] can't be accessed from runtime function [[%s]]");
    SWAG_ERROR(Err0176, "invalid access from runtime                       $ the compile-time function [[%s]] can't be accessed from runtime function [[%s]]");
    SWAG_ERROR(Err0177, "invalid access from runtime                       $ this compile-time constant can't be accessed from runtime");
    SWAG_ERROR(Err0178, "invalid address                                   $ can't take the address because the function returns nothing");
    SWAG_ERROR(Err0179, "invalid address                                   $ can't take the address of %s $ this is not a variable");
    SWAG_ERROR(Err0180,
               "invalid address                                   $ can't take the address of a constant of type [[%s]] $ this type of constant does not have an associated memory storage");
    SWAG_ERROR(Err0181,
               "invalid address                                   $ can't take the address of a function parameter of type [[%s]] $ this type of parameter does not have an associated memory storage");
    SWAG_ERROR(Err0182, "invalid address                                   $ can't take the address of a function returned value of type [[%s]]");
    SWAG_ERROR(Err0183, "invalid address                                   $ can't take the address of a macro");
    SWAG_ERROR(Err0184, "invalid address                                   $ can't take the address of a mixin");
    SWAG_ERROR(Err0185, "invalid address                                   $ can't take the address of a variable declared with [[let]]");
    SWAG_ERROR(Err0186, "invalid address                                   $ can't take the address of an in-lined function");
    SWAG_ERROR(Err0187, "invalid address                                   $ can't take the address of the right expression");
    SWAG_ERROR(Err0188, "invalid alias name                                $ expected an identifier for name aliasing, found [[%s]] instead");
    SWAG_ERROR(Err0189, "invalid alias name                                $ expected the alias name after [[%s]], found [[%s]] instead");
    SWAG_ERROR(Err0190, "invalid align value                               $ the [[#[Swag.Align]]] argument must be a power of two, got [[%d]]");
    SWAG_ERROR(Err0191, "invalid argument                                  $ [[%s]] requires an argument of type [[bool]], got [[%s]] instead");
    SWAG_ERROR(Err0192, "invalid argument                                  $ [[#include]] requires an argument of type [[string]], got [[%s]] instead");
    SWAG_ERROR(Err0193, "invalid argument                                  $ [[#mixin]] requires an argument of type [[code]], got [[%s]] instead");
    SWAG_ERROR(Err0194, "invalid argument                                  $ the [[%s]] count must be an integer, got [[%s]] instead");
    SWAG_ERROR(Err0195, "invalid argument                                  $ the intrinsic [[%s]] requires a block pointer as a first argument, got [[%s]] instead");
    SWAG_ERROR(Err0196, "invalid argument                                  $ the intrinsic [[%s]] requires a pointer as a first argument, got [[%s]] instead");
    SWAG_ERROR(Err0197,
               "invalid argument                                  $ the intrinsic [[%s]] requires a pointer to memory as a first argument if the [[count]] is greater than one (type is [[%s]])");
    SWAG_ERROR(Err0198,
               "invalid argument                                  $ the intrinsic [[%s]] requires a pointer to memory as a first argument if the [[count]] is variable (type is [[%s]])");
    SWAG_ERROR(Err0199,
               "invalid argument                                  $ the intrinsic [[%s]] requires a variable name as a first argument if the second argument is not specified");
    SWAG_ERROR(Err0200, "invalid argument                                  $ the intrinsic [[%s]] requires an argument of type [[string]], got [[%s]] instead");
    SWAG_ERROR(Err0201, "invalid argument                                  $ the intrinsic [[@gettag]] expects a default value of type [[%s]] as the last argument");
    SWAG_ERROR(Err0202, "invalid argument                                  $ the intrinsic [[@mkany]] can't have [[null]] as a first argument");
    SWAG_ERROR(Err0203, "invalid argument                                  $ the intrinsic [[@mkany]] expects a type as the second argument, got [[%s]] instead");
    SWAG_ERROR(Err0204, "invalid argument                                  $ the intrinsic [[@mkcallback]] requires a pointer to function argument, got [[%s]] instead");
    SWAG_ERROR(Err0205, "invalid argument                                  $ the intrinsic [[@mkinterface]] requires a pointer or a struct as first argument, got [[%s]] instead");
    SWAG_ERROR(Err0206, "invalid argument                                  $ the intrinsic [[@mkinterface]] requires a type as a second argument, got [[%s]] instead");
    SWAG_ERROR(Err0207, "invalid argument                                  $ the intrinsic [[@mkinterface]] requires an interface name as a third argument, got [[%s]] instead");
    SWAG_ERROR(Err0208, "invalid argument                                  $ the intrinsic [[@mkstring]] requires a pointer to [[u8]] as a first argument, got [[%s]] instead");
    SWAG_ERROR(Err0209, "invalid argument                                  $ the intrinsic [[@runes]] requires an argument of type [[string]], got [[%s]] instead");
    SWAG_ERROR(Err0210, "invalid array dimension                           $ the array dimension is 0");
    SWAG_ERROR(Err0211, "invalid array dimension                           $ the array dimension must be an integer, got [[%s]] instead");
    SWAG_ERROR(Err0212, "invalid array size                                $ the size of the array can't be deduced because of a missing initialization");
    SWAG_ERROR(Err0213, "invalid assign                                    $ a function pointer can't be assigned to a constant");
    SWAG_ERROR(Err0214, "invalid assign                                    $ assignments are not allowed for unnamed parameters ([[?]])");
    SWAG_ERROR(Err0215, "invalid assign                                    $ can't assign because the left expression isn't a value");
    SWAG_ERROR(Err0216, "invalid assign                                    $ the affectation to the tuple is not possible because the right expression is not compatible");
    SWAG_ERROR(Err0217, "invalid attribute                                 $ [[%s]] is not an attribute but is %s");
    SWAG_ERROR(Err0218, "invalid attribute                                 $ expected an attribute, but [[%s]] is %s");
    SWAG_ERROR(Err0219, "invalid attribute name                            $ expected an attribute name, found [[%s]] instead");
    SWAG_ERROR(Err0220, "invalid attribute usage                           $ expected [[,]] for a new attribute name or [[]]] to end the list, found [[%s]] instead");
    SWAG_ERROR(Err0221,
               "invalid attribute usage                           $ the [[AttributeUsage.Gen]] can only be associated with [[AttributeUsage.Struct]] or [[AttributeUsage.Enum]]");
    SWAG_ERROR(Err0222, "invalid attribute use                             $ a %s can't have the [[#[Swag.CalleeReturn]]] attribute");
    SWAG_ERROR(Err0223, "invalid attribute use                             $ a %s can't have the [[#[Swag.Inline]]] attribute");
    SWAG_ERROR(Err0224, "invalid attribute use                             $ a %s can't have the [[#[Swag.Macro]]] attribute");
    SWAG_ERROR(Err0225, "invalid attribute use                             $ a %s can't have the [[#[Swag.Mixin]]] attribute");
    SWAG_ERROR(Err0226, "invalid attribute use                             $ a %s can't have the [[#[Swag.NotGeneric]]] attribute");
    SWAG_ERROR(Err0227, "invalid binary number format                      $ binary literals should only contain [[0]] or [[1]], found [[%s]] instead");
    SWAG_ERROR(Err0228, "invalid bit-cast                                  $ bit-cast from type [[%s]] is not allowed $ expected an integer, a rune, a float or a pointer");
    SWAG_ERROR(Err0229, "invalid bit-cast                                  $ bit-cast to a larger type is not allowed ([[%s]] to [[%s]])");
    SWAG_ERROR(Err0230, "invalid bit-cast                                  $ bit-cast to type [[%s]] is not allowed $ expected integer, rune, or float");
    SWAG_ERROR(Err0231, "invalid block start                               $ a block start [[{]] is not allowed after [[discard try/assume/catch]]");
    SWAG_ERROR(Err0232, "invalid capture                                   $ can't capture [[%s]] because it's %s $ capturing %s type is not supported");
    SWAG_ERROR(Err0233,
               "invalid capture                                   $ can't capture [[%s]] because it's not a plain old data struct $ a struct is not plain old data if it contains [[opDrop]], [[opPostCopy]] or [[opPostMove]]");
    SWAG_ERROR(Err0234, "invalid character                                 $ the character [[%s]] is not recognized in this context");
    SWAG_ERROR(Err0235, "invalid character literal                         $ can't convert a character literal to type [[%s]]");
    SWAG_ERROR(Err0236, "invalid character literal                         $ the character literal [[%s]] seems to be a string and not a character");
    SWAG_ERROR(Err0237,
               "invalid character syntax                          $ characters should be delimited with back-ticks [[`]] and not quotes $ consider using the syntax [[`%s`]] instead");
    SWAG_ERROR(Err0238, "invalid comparison                                $ unexpected comparison of a slice and [[%s]] $ a slice comparison is only allowed with [[null]]");
    SWAG_ERROR(Err0239,
               "invalid comparison                                $ unexpected comparison of type [[any]] and [[%s]] $ an [[any]] comparison is only allowed with [[null]] or with a type");
    SWAG_ERROR(Err0240,
               "invalid comparison                                $ unexpected comparison of an interface and [[%s]] $ an interface comparison is only allowed with [[null]], another interface or with a type");
    SWAG_ERROR(Err0241, "invalid comparison                                $ comparison operations on tuples are not supported");
    SWAG_ERROR(Err0242, "invalid comparison                                $ the comparison operation [[%s]] does not accept the type [[%s]] as the left argument");
    SWAG_ERROR(Err0243, "invalid comparison                                $ the comparison operation [[%s]] does not accept the type [[%s]] as the right argument");
    SWAG_ERROR(Err0244, "invalid comparison                                $ the comparison operation [[%s]] does not accept the type [[%s]]");
    SWAG_ERROR(Err0245, "invalid compiler directive                        $ [[%s]] is not a valid compiler directive starting with [[#]]");
    SWAG_ERROR(Err0246, "invalid compound name                             $ expected a single identifier %s $ consider using a single name without the [[.]] character");
    SWAG_ERROR(Err0247, "invalid const [[&&]]                              $ the [[&&]] move reference type can't be combined with an immutable ([[const]]) expression");
    SWAG_ERROR(Err0248, "invalid constant                                  $ the constant creation failed due to the generic type [[%s]]");
    SWAG_ERROR(Err0249, "invalid constant name                             $ expected the constant name after [[const]], found [[%s]] instead");
    SWAG_ERROR(Err0250, "invalid default parameter                         $ [[%s]] can't be used as a default parameter");
    SWAG_ERROR(Err0251, "invalid default parameters                        $ the default parameters of a lambda or a closure can't be redefined");
    SWAG_ERROR(Err0252, "invalid default value                             $ [[self]] can't have a default value");
    SWAG_ERROR(Err0253, "invalid default value                             $ lambda parameters can't have a default value");
    SWAG_ERROR(Err0254, "invalid dependency                                $ can't resolve the module dependency [[%s]] because the [[location]] is empty");
    SWAG_ERROR(Err0255, "invalid dependency                                $ the dependency module folder [[%s]] does not exist");
    SWAG_ERROR(Err0256, "invalid dereference                               $ [[%s]] has type [[%s]] and can't be dereferenced by index because pointer arithmetic is not allowed");
    SWAG_ERROR(Err0257, "invalid dereference                               $ [[%s]] has type [[%s]] and can't be dereferenced like a struct or a pointer to struct");
    SWAG_ERROR(Err0258, "invalid dereference                               $ can't dereference [[%s]] by index because the special function [[opIndex]] can't be found in [[%s]]");
    SWAG_ERROR(Err0259, "invalid dereference                               $ can't dereference by index because the special function [[opIndex]] can't be found in [[%s]]");
    SWAG_ERROR(Err0260, "invalid dereference                               $ dereferencing type [[%s]] is not valid");
    SWAG_ERROR(Err0261, "invalid embedded enum                             $ expected an enum type, got [[%s]] instead");
    SWAG_ERROR(Err0262, "invalid embedded instruction                      $ expected an embedded instruction or a curly block, found [[%s]] instead");
    SWAG_ERROR(Err0263, "invalid empty attribute                           $ an attribute should contain at least one value $ you should specify the attribute name");
    SWAG_ERROR(Err0264, "invalid empty statement [[;]]                     $ a semicolon [[;]] can't be used alone to mark an empty statement");
    SWAG_ERROR(Err0265, "invalid enum identifier                           $ expected an enum value identifier, found [[%s]] instead");
    SWAG_ERROR(Err0266, "invalid enum name                                 $ expected the enum name, found [[%s]] instead");
    SWAG_ERROR(Err0267, "invalid enum type                                 $ an enum marked with [[#[Swag.EnumFlags]]] should have an unsigned integer type, got [[%s]] instead");
    SWAG_ERROR(Err0268, "invalid enum type                                 $ an enum marked with [[#[Swag.EnumIndex]]] should have an integer type, got [[%s]] instead");
    SWAG_ERROR(Err0269, "invalid enum type                                 $ an enum marked with [[#[Swag.NoDuplicate]]] does not accept type [[%s]]");
    SWAG_ERROR(Err0270, "invalid enum type                                 $ the enum array type [[%s]] should be [[const]]");
    SWAG_ERROR(Err0271, "invalid enum type                                 $ the enum array type [[%s]] should have specified dimensions");
    SWAG_ERROR(Err0272,
               "invalid enum type                                 $ the enum slice type [[%s]] should be declared as [[const]] $ consider declaring the type with [[const %s]] instead");
    SWAG_ERROR(Err0273, "invalid enum type                                 $ the type [[%s]] is not a valid enum type");
    SWAG_ERROR(Err0274, "invalid enum type                                 $ the type [[cstring]] is not a valid enum type $ consider using [[string]] instead");
    SWAG_ERROR(Err0275, "invalid escape code                               $ [[%c]] is not a valid escape code");
    SWAG_ERROR(Err0276, "invalid escape format                             $ the [[\\U]] escape code requires 8 hexadecimal digits");
    SWAG_ERROR(Err0277, "invalid escape format                             $ the [[\\u]] escape code requires 4 hexadecimal digits");
    SWAG_ERROR(Err0278, "invalid escape format                             $ the [[\\x]] escape code requires 2 hexadecimal digits");
    SWAG_ERROR(Err0279, "invalid expand                                    $ can't expand [[%s]] in global scope");
    SWAG_ERROR(Err0280,
               "invalid export argument                           $ invalid argument [[%s]] for the attribute [[#[Swag.ExportType]]] $ the acceptable values for [[#[Swag.ExportType]]] are [[methods]] and [[nozero]]");
    SWAG_ERROR(Err0281, "invalid expression                                $ expected an expression after %s, found [[%s]] instead");
    SWAG_ERROR(Err0282,
               "invalid expression                                $ expected an expression after operator [[%s]], found [[%s]] instead $ consider adding [[(]] after [[%s]] to start an expression");
    SWAG_ERROR(Err0283, "invalid expression                                $ expected an expression, found [[%s]] instead");
    SWAG_ERROR(Err0284, "invalid expression                                $ expected an expression, found a type instead");
    SWAG_ERROR(Err0285, "invalid expression                                $ the directive [[%s]] can't be used as an expression $ this directive can only be used as a statement");
    SWAG_ERROR(Err0286, "invalid float number format                       $ exponent in the scientific notation must contain at least one digit");
    SWAG_ERROR(Err0287, "invalid float number format                       $ the exponent part should not start with [[_]]");
    SWAG_ERROR(Err0288, "invalid float number format                       $ the fractional part should not start with [[_]]");
    SWAG_ERROR(Err0289, "invalid function call                             $ [[%s]] has call parameters but is %s, not a function");
    SWAG_ERROR(Err0290, "invalid function call                             $ [[%s]] has call parameters but the type [[%s]] is not a function");
    SWAG_ERROR(Err0291, "invalid function call                             $ call to function [[%s]] requires arguments between [[()]] and not [[{}]]");
    SWAG_ERROR(Err0292, "invalid function call                             $ can't call [[%s]] because the function is declared but not defined");
    SWAG_ERROR(Err0293,
               "invalid function declaration                      $ expected [[func]] or [[mtd]] to declare an interface function, found [[%s]] instead $ to declare a function within an interface, use [[func]] or [[mtd]]");
    SWAG_ERROR(Err0294, "invalid function name                             $ a function name ([[%s]]) can't start with [[@]], this is reserved for intrinsics");
    SWAG_ERROR(Err0295, "invalid function name                             $ expected a function name, found [[%s]]");
    SWAG_ERROR(Err0296,
               "invalid generated [[impl]] block                  $ [[impl]] block generation is permissible only within a [[#message]] with [[Swag.CompilerMsgMask.AttributeGen]]");
    SWAG_ERROR(Err0297, "invalid generic                                   $ a generic struct instantiation of a partial type alias ([[%s]]) is not supported");
    SWAG_ERROR(Err0298, "invalid generic                                   $ can't deduce the generic arguments when calling %s [[%s]]");
    SWAG_ERROR(Err0299, "invalid generic                                   $ can't deduce the generic type [[%s]] from type [[%s]]");
    SWAG_ERROR(Err0300, "invalid generic                                   $ can't instantiate the generic function [[%s]] because of missing contextual types replacements");
    SWAG_ERROR(Err0301, "invalid generic                                   $ can't instantiate the generic struct [[%s]] because of missing contextual types replacements");
    SWAG_ERROR(Err0302,
               "invalid generic                                   $ the generic value [[%s]] has already been deduced from arguments to be [[%s]], and [[%s]] is provided here");
    SWAG_ERROR(Err0303, "invalid generic argument                          $ invalid usage of %s ([[%s]]) as a generic argument $ a generic argument must be a literal or a type");
    SWAG_ERROR(Err0304, "invalid generic argument                          $ a type is expected but got a value instead");
    SWAG_ERROR(Err0305, "invalid generic argument                          $ a value is expected but got a type instead");
    SWAG_ERROR(Err0306, "invalid generic parameter                         $ the special function [[%s]] expects a literal as a generic parameter, got type [[%s]] instead");
    SWAG_ERROR(Err0307, "invalid generic parameter name                    $ expected an identifier as the generic name, found [[%s]] instead");
    SWAG_ERROR(Err0308, "invalid hexadecimal number format                 $ hexadecimal literals can only contain digits [[0-9]], [[A-F]], and [[a-f]], found [[%s]] instead");
    SWAG_ERROR(Err0309, nullptr);
    SWAG_ERROR(Err0310, "invalid identifier                                $ expected an identifier, found [[%s]] instead");
    SWAG_ERROR(Err0311, "invalid identifier list                           $ [[with]] does not support multiple identifiers");
    SWAG_ERROR(Err0312, "invalid import version                            $ the [[#import]] version format is invalid");
    SWAG_ERROR(Err0313, "invalid initialization                            $ can't initialize a constant array (type is [[%s]]) with one single value");
    SWAG_ERROR(Err0314, "invalid interface conversion                      $ [[%s]] (or a [[using]] field) doesn't implement [[%s]], so struct-to-interface cast is not allowed");
    SWAG_ERROR(Err0315,
               "invalid interface function                        $ the first parameter should be [[self]], got [[%s]] instead $ consider declaring the interface function with [[mtd]] instead of [[func]]");
    SWAG_ERROR(Err0316, "invalid intrinsic                                 $ [[%s]] is not a valid intrinsic starting with [[@]]");
    SWAG_ERROR(Err0317, "invalid lambda call                               $ invalid lambda call because [[%s]] is not a variable, it is %s");
    SWAG_ERROR(Err0318, "invalid literal                                   $ invalid number conversion for literal [[%I64u]]");
    SWAG_ERROR(Err0319, "invalid literal suffix                            $ the symbol [[%s]] is %s and can't be used as a literal type suffix");
    SWAG_ERROR(Err0320, "invalid literal suffix                            $ the type [[%s]] can't be used as a literal type suffix");
    SWAG_ERROR(Err0321, "invalid literal suffix                            $ the type [[%s]] can't be used as suffix for a float literal $ only [[f32]] and [[f64]] are accepted");
    SWAG_ERROR(Err0322, "invalid location argument                         $ invalid [[location]] mode [[%s]] $ the acceptable values are [[swag]] and [[disk]]");
    SWAG_ERROR(Err0323, "invalid logical operator                          $ expected [[%s]] for the logical test, found [[%s]] instead");
    SWAG_ERROR(Err0324,
               "invalid match argument                            $ invalid argument [[%s]] for the attribute [[#[Swag.Match]]] $ the acceptable values for [[#[Swag.Match]]] are [[validif]] and [[self]]");
    SWAG_ERROR(Err0325, "invalid module name                               $ the [[#[Swag.Foreign]]] module name can't be empty");
    SWAG_ERROR(Err0326,
               "invalid module name                               $ unexpected symbol [[.]] in the [[#[Swag.Foreign]]] module name $ a [[#[Swag.Foreign]]] module name can't have an extension, consider removing it");
    SWAG_ERROR(Err0327, "invalid move                                      $ [[move]] is not applicable on an immutable expression (type is [[%s]])");
    SWAG_ERROR(Err0328, "invalid name alias                                $ a name alias should not be used on %s");
    SWAG_ERROR(Err0329, "invalid named argument                            $ expected a named argument before [[:]], found [[%s]] instead");
    SWAG_ERROR(Err0330, "invalid namespace name                            $ expected the namespace name, found [[%s]] instead");
    SWAG_ERROR(Err0331, "invalid negation                                  $ can't negate an unsigned type [[%s]]");
    SWAG_ERROR(Err0332, "invalid negation                                  $ can't negate type [[%s]]");
    SWAG_ERROR(Err0333, "invalid number format                             $ a number should not end with a digit separator [[_]]");
    SWAG_ERROR(Err0334, "invalid number format                             $ a number should not start with a digit separator [[_]]");
    SWAG_ERROR(Err0335, "invalid number format                             $ consecutive digit separators [[_]] are not allowed");
    SWAG_ERROR(Err0336, "invalid number format                             $ this binary number has too many digits to fit within a 64-bit representation");
    SWAG_ERROR(Err0337, "invalid number format                             $ this hexadecimal number has too many digits to fit within a 64-bit representation");
    SWAG_ERROR(Err0338, "invalid number prefix                             $ expected [[0x]] for hexadecimal or [[0b]] for binary, found [[%s]] instead");
    SWAG_ERROR(Err0339, "invalid opaque struct                             $ a struct marked with [[#[Swag.Opaque]]] requires a [[public]] access");
    SWAG_ERROR(Err0340,
               "invalid opaque struct                             $ the struct can't be marked with [[#[Swag.Opaque]]] because the whole file is exported with [[#global export]]");
    SWAG_ERROR(Err0341,
               "invalid operation                                 $ can't affect by index a type [[%s]] to [[%s]] because no corresponding [[opIndexAffect]] can be found");
    SWAG_ERROR(Err0342, "invalid operation                                 $ can't initialize a type [[%s]] with [[%s]] because no corresponding [[opAffectSuffix]] can be found");
    SWAG_ERROR(Err0343, "invalid operation                                 $ missing special function [[%s]] in [[%s]] when solving operator [[%s]]");
    SWAG_ERROR(Err0344, "invalid operation                                 $ missing special function [[%s]] in [[%s]]");
    SWAG_ERROR(Err0345, "invalid operation                                 $ the bit inversion operation [[~]] is not allowed on type [[%s]]");
    SWAG_ERROR(Err0346, "invalid operation                                 $ the operation [[%s]] does not accept the type [[%s]] as the left argument");
    SWAG_ERROR(Err0347, "invalid operation                                 $ the operation [[%s]] does not accept the type [[%s]] as the right argument");
    SWAG_ERROR(Err0348,
               "invalid operation                                 $ the operation [[%s]] is not allowed because the left expression is an enum type ([[%s]]) not marked with [[#[Swag.EnumFlags]]]");
    SWAG_ERROR(Err0349,
               "invalid operation                                 $ the operation [[%s]] is not allowed because the right expression is an enum type ([[%s]]) not marked with [[#[Swag.EnumFlags]]]");
    SWAG_ERROR(Err0350, "invalid operation                                 $ the operation [[%s]] is not allowed on tuple types");
    SWAG_ERROR(Err0351, "invalid operation                                 $ the operation [[%s]] is not allowed on type [[%s]]");
    SWAG_ERROR(Err0352, "invalid operator modifier                         $ [[%s]] is not recognized as a valid instruction modifier");
    SWAG_ERROR(Err0353,
               "invalid argument                                  $ invalid argument [[%s]] for the attribute [[#[Swag.Optim]]] $ the acceptable values for [[#[Swag.Optim]]] are [[bytecode]] and [[backend]]");
    SWAG_ERROR(Err0354, "invalid packing value                             $ the [[#[Swag.Pack]]] argument must be 0 or a power of two, got [[%d]]");
    SWAG_ERROR(Err0355, "invalid parameter                                 $ the [[#message]] block needs a parameter of type [[Swag.CompilerMsgMask]], got [[%s]]");
    SWAG_ERROR(Err0356, "invalid parameter name                            $ expected a parameter name, found [[%s]] instead");
    SWAG_ERROR(Err0357, "invalid pass name                                 $ [[%s]] is not recognized as a valid compiler pass name");
    SWAG_ERROR(Err0358, "invalid pointer arithmetic                        $ pointer arithmetic is not allowed on a pointer to [[void]] $ [[void]] does not have a valid size");
    SWAG_ERROR(Err0359, "invalid pointer arithmetic                        $ pointer arithmetic is not allowed");
    SWAG_ERROR(Err0360, "invalid pointer arithmetic                        $ pointer arithmetic requires an integer, got [[%s]] instead");
    SWAG_ERROR(Err0361, "invalid pointer arithmetic                        $ pointer slicing is not allowed");
    SWAG_ERROR(Err0362, "invalid range                                     $ the lower bound [[%I64u]] is greater than the upper bound [[%I64u]]");
    SWAG_ERROR(Err0363, "invalid range                                     $ the lower bound [[%lld]] is greater than the upper bound [[%lld]]");
    SWAG_ERROR(Err0364, "invalid range type                                $ unexpected type [[%s]] in a range expression");
    SWAG_ERROR(Err0365, "invalid reference                                 $ [[ref]] requires a pointer or a reference as an expression, got [[%s]] instead");
    SWAG_ERROR(Err0366, "invalid reference                                 $ invalid reference to the current file scope name");
    SWAG_ERROR(Err0367, "invalid reference to [[with]] block               $ an identifier is expected after [[.]], found [[%s]] instead");
    SWAG_ERROR(Err0368, "invalid return type                               $ a function can't return type [[%s]]");
    SWAG_ERROR(Err0369,
               "invalid return type                               $ a function return of type [[void]] is invalid $ just omit the return type if you want the function to return nothing");
    SWAG_ERROR(Err0370, "invalid return type                               $ the function [[opSlice]] must return a string or a slice, got [[%s]] instead");
    SWAG_ERROR(Err0371, "invalid return value assignment                   $ the call does not return a value");
    SWAG_ERROR(Err0372, "invalid runtime typeinfo                          $ failed to convert [[%s]] to a runtime typeinfo");
    SWAG_ERROR(Err0373,
               "invalid safety argument                           $ invalid safety value [[%s]] $ the acceptable safety values are [[boundcheck|overflow|math|switch|unreachable|any|bool|nan|sanity|null]]");
    SWAG_ERROR(Err0374, "invalid slicing                                   $ can't slice [[%s]] because [[opSlice]] was not found in type [[%s]]");
    SWAG_ERROR(Err0375, "invalid slicing                                   $ slicing is invalid for type [[%s]]");
    SWAG_ERROR(Err0376, "invalid slicing                                   $ slicing is invalid on a multi-dimensional array");
    SWAG_ERROR(Err0377, "invalid struct initialization                     $ expected [[{}]] to initialize a struct, and not [[()]] which is reserved for function calls");
    SWAG_ERROR(Err0378, "invalid struct name                               $ expected the struct name, found [[%s]] instead");
    SWAG_ERROR(Err0379, "invalid throw type                                $ throw needs a struct or [[@err()]] as an expression, but there's nothing here");
    SWAG_ERROR(Err0380, "invalid throw type                                $ throw needs a struct or [[@err()]] as an expression, got [[%s]] instead");
    SWAG_ERROR(Err0381, "invalid top-level instruction                     $ expected a top-level instruction, found [[%s]] instead");
    SWAG_ERROR(Err0382, "invalid tuple dereference                         $ tuples can't be dereferenced like pointers");
    SWAG_ERROR(Err0383, "invalid tuple unpacking                           $ can't unpack an empty tuple");
    SWAG_ERROR(Err0384, "invalid tuple unpacking                           $ can't unpack type [[%s]] $ a struct or a tuple is expected");
    SWAG_ERROR(Err0385, "invalid type                                      $ the affect expression has type [[void]]");
    SWAG_ERROR(Err0386, "invalid type                                      $ the initialization expression has type [[void]]");
    SWAG_ERROR(Err0387, "invalid type                                      $ the intrinsic [[@spread]] can't be used with type [[%s]] $ this kind of type can't be spreaded");
    SWAG_ERROR(Err0388, "invalid type                                      $ the operator [[<<]] requires an integer, got [[%s]] instead");
    SWAG_ERROR(Err0389, "invalid type                                      $ the operator [[>>]] requires an integer, got [[%s]] instead");
    SWAG_ERROR(Err0390,
               "invalid type                                      $ the second parameter of [[%s]] can't be of type [[%s]] $ consider using [[opAssign]] if you want to make a copy");
    SWAG_ERROR(Err0391, "invalid type                                      $ the special function [[%s]] expects a [[bool]] as a generic parameter, got [[%s]] instead");
    SWAG_ERROR(Err0392, "invalid type                                      $ the special function [[%s]] expects a [[string]] as a generic parameter, got [[%s]] instead");
    SWAG_ERROR(Err0393, "invalid type                                      $ the type [[%s]] can't be used as a type for an attribute parameter");
    SWAG_ERROR(Err0394, "invalid type                                      $ the type [[%s]] can't be used as an argument to [[@cvaarg]]");
    SWAG_ERROR(Err0395, "invalid type                                      $ the type [[%s]] does not accept a [[const]] version");
    SWAG_ERROR(Err0396, "invalid type                                      $ the type is generic and can't be evaluated in this context");
    SWAG_ERROR(Err0397, "invalid type                                      $ unexpected first parameter type for special function [[%s]] ([[%s]] expected, [[%s]] provided)");
    SWAG_ERROR(Err0398, "invalid type constraint                           $ a type constraint should be a boolean expression, got type [[%s]] instead");
    SWAG_ERROR(Err0399, "invalid type declaration                          $ expected a type but [[%s]] is %s");
    SWAG_ERROR(Err0400, "invalid type declaration                          $ expected a type declaration, found [[%s]] instead");
    SWAG_ERROR(Err0401, "invalid type declaration                          $ expected a type in the tuple definition, found [[%s]] instead");
    SWAG_ERROR(Err0402, "invalid type declaration                          $ expected the array type after its dimensions, found [[%s]] instead");
    SWAG_ERROR(Err0403, "invalid type suffix                               $ a literal with a suffix (in that case [[%s]]) should only be used for a struct conversion");
    SWAG_ERROR(Err0404, "invalid type suffix                               $ expected an identifier or a type after the start of a type suffix, found [[%s]] instead");
    SWAG_ERROR(Err0405, "invalid unicode value                             $ value [[0x%x is not a valid unicode code point, and can't be converted to UTF8");
    SWAG_ERROR(Err0406, "invalid variable list                             $ [[if]] does not support multiples variable declarations");
    SWAG_ERROR(Err0407,
               "invalid variable name                             $ a variable name ([[%s]]) can't start with [[@]], this is reserved for intrinsics $ only [[#mixin]] and [[#alias]] are possible in that case");
    SWAG_ERROR(Err0408, "invalid variable name                             $ expected a variable name, found [[%s]] instead");
    SWAG_ERROR(Err0409, "invalid variable name                             $ expected the variable name after [[%s]], found [[%s]] instead");
    SWAG_ERROR(Err0410, "invalid variable name                             $ unexpected generic parameters after variable name [[%s]]");
    SWAG_ERROR(Err0411, "invalid variable name                             $ unexpected parameters after variable name [[%s]]");
    SWAG_ERROR(Err0412, "invalid variable type                             $ a variable can't be declared with a [[void]] type");
    SWAG_ERROR(Err0413, "invalid variable type                             $ failed to deduce the type of %s [[%s]]");
    SWAG_ERROR(Err0414, "invalid variable type                             $ failed to deduce the type of the variable because the expression is [[null]]");
    SWAG_ERROR(Err0415, "invalid visit                                     $ can't visit a pointer of type [[%s]]");
    SWAG_ERROR(Err0416, "invalid visit                                     $ can't visit a variadic type by pointer");
    SWAG_ERROR(Err0417, "invalid visit                                     $ can't visit an enum content by pointer");
    SWAG_ERROR(Err0418, "invalid visit                                     $ can't visit type [[%s]]");
    SWAG_ERROR(Err0419, "invalid visit                                     $ can't find the [[%s]] visit specialization in [[%s]]");
    SWAG_ERROR(Err0420, "invalid visit                                     $ the type [[%s]] can't be visited with [[visit]]");
    SWAG_ERROR(Err0421, "invalid void dereference                          $ dereferencing a pointer to [[void]] is not valid $ [[void]] does not have a valid size");
    SWAG_ERROR(Err0422, "linker error                                      $ the LLVM backend failed to create the target [[%s]]");
    SWAG_ERROR(Err0423, "literal overflow                                  $ can't negate number [[%I64d]] because [[%I64u]] is too large for type [[s64]]");
    SWAG_ERROR(Err0424, "literal overflow                                  $ can't negate number [[%d]] because [[%u]] is too large for type [[s8]]");
    SWAG_ERROR(Err0425, "literal overflow                                  $ the number [[%I64u]] is too large for type [[%s]]");
    SWAG_ERROR(Err0426, "mismatch access                                   $ %s [[%s]] can't be public because %s [[%s]] has [[%s]] access");
    SWAG_ERROR(Err0427, "mismatch access                                   $ the special function [[%s]] can't be internal because the corresponding struct has [[public]] access");
    SWAG_ERROR(Err0428, "mismatch access                                   $ the special function [[%s]] can't be public because the corresponding struct has [[internal]] access");
    SWAG_ERROR(Err0429, "mismatch enum types                               $ expected an enum of type [[%s]], got [[%s]] instead");
    SWAG_ERROR(Err0430, "mismatch function signature                       $ function [[%s]] has an incorrect signature for interface [[%s]]");
    SWAG_ERROR(Err0431, "mismatch scopes                                   $ the [[impl]] block is not defined in the same scope as [[%s]]");
    SWAG_ERROR(Err0432, "misplaced [[#dependencies]]                       $ [[#dependencies]] can only be used within [[module.swg]] or [[.swgs]] files");
    SWAG_ERROR(Err0433, "misplaced [[#dependencies]]                       $ [[#dependencies]] must be placed at the file level");
    SWAG_ERROR(Err0434, "misplaced [[#do]]                                 $ unnecessary usage of [[#do]] before a block start [[{]] $ consider removing it");
    SWAG_ERROR(Err0435, "misplaced [[#global testerror]]                   $ [[#global testerror]] is only valid in a test file from the [[./tests]] folder");
    SWAG_ERROR(Err0436, "misplaced [[#global testwarning]]                 $ [[#global testwarning]] is only valid in a test file from the [[./tests]] folder");
    SWAG_ERROR(Err0437, "misplaced [[#global using]]                       $ [[#global using]] can only be used within [[module.swg]] or [[.swgs]] files");
    SWAG_ERROR(Err0438, "misplaced [[#global]]                             $ [[#global]] should be placed at the very top of the file");
    SWAG_ERROR(Err0439, "misplaced [[#import]]                             $ [[#import]] can only be used within [[module.swg]] or [[.swgs]] files");
    SWAG_ERROR(Err0440, "misplaced [[#import]]                             $ [[#import]] must be placed within the [[#dependencies]] block");
    SWAG_ERROR(Err0441,
               "misplaced [[#index]]                              $ the keyword [[#index]] is only valid within a breakable block such as [[loop]], [[visit]], [[for]], etc.");
    SWAG_ERROR(Err0442, "misplaced [[#load]]                               $ [[#load]] can only be used within [[module.swg]] or [[.swgs]] files");
    SWAG_ERROR(Err0443, "misplaced [[#load]]                               $ [[#load]] must be placed within the [[#dependencies]] block");
    SWAG_ERROR(Err0444, "misplaced [[#macro]]                              $ [[#macro]] can only be used within a [[#[Swag.Macro]]] function");
    SWAG_ERROR(Err0445, "misplaced [[#mixin]] block                        $ a [[#mixin]] block is only valid within a breakable block such as [[loop]], [[visit]], [[for]], etc.");
    SWAG_ERROR(Err0446, "misplaced [[#placeholder]]                        $ [[#placeholder]] must be placed within a top level scope");
    SWAG_ERROR(Err0447, "misplaced [[#self]]                               $ [[#self]] can only be used within functions");
    SWAG_ERROR(Err0448, "misplaced [[#test]]                               $ #test]] functions can only be declared in test modules (in the [[./tests]] folder)");
    SWAG_ERROR(Err0449, "misplaced [[#up]]                                 $ [[#up]] can only be used within a [[#[Swag.Macro]]] function");
    SWAG_ERROR(Err0450, "misplaced [[%s]]                                  $ [[%s]] can only be used inside a function marked with [[throw]], but [[%s]] isn't");
    SWAG_ERROR(Err0451, "misplaced [[%s]]                                  $ [[%s]] can't be used in a struct special function ([[%s]])");
    SWAG_ERROR(Err0452,
               "misplaced [[@cvastart]]                           $ the intrinsic [[@cvastart]] can only be used in a function with a final parameter of type [[cvarargs]]");
    SWAG_ERROR(Err0453, "misplaced [[@spread]]                             $ the intrinsic [[@spread]] can only be used as a function argument");
    SWAG_ERROR(Err0454, "misplaced [[Self]] type                           $ the [[Self]] type is only valid within an [[impl]], [[struct]], or [[interface]] block");
    SWAG_ERROR(Err0455, "misplaced [[break]]                               $ [[break]] is only valid within a breakable block such as [[loop]], [[visit]], [[for]], etc.");
    SWAG_ERROR(Err0456, "misplaced [[code]]                                $ [[code]] is only valid within a [[#[Swag.Macro]]] or [[#[Swag.Mixin]]] function");
    SWAG_ERROR(Err0457, "misplaced [[const]]                               $ expected [[self]] after [[const]] when declaring a function parameter, found [[%s]] instead");
    SWAG_ERROR(Err0458, "misplaced [[const]] qualifier                     $ the [[const]] qualifier is not allowed for [[func]], only for [[mtd]]");
    SWAG_ERROR(Err0459, "misplaced [[continue]]                            $ [[continue]] is only valid within a breakable block such as [[loop]], [[visit]], [[for]], etc.");
    SWAG_ERROR(Err0460, "misplaced [[do]]                                  $ unnecessary usage of [[do]] before a block start [[{]] $ consider removing it");
    SWAG_ERROR(Err0461, "misplaced [[fallthrough]]                         $ [[fallthrough]] is invalid in the last [[case]] of a [[switch]]");
    SWAG_ERROR(Err0462, "misplaced [[fallthrough]]                         $ [[fallthrough]] is only valid within a [[case]] block");
    SWAG_ERROR(Err0463, "misplaced [[fallthrough]]                         $ [[fallthrough]] is only valid within a [[switch]]");
    SWAG_ERROR(Err0464, "misplaced [[moveref]]                             $ unexpected [[moveref]] on a computed value $ [[moveref]] should be used on a pointer or a reference");
    SWAG_ERROR(Err0465, "misplaced [[moveref]]                             $ unexpected [[moveref]] on an immutable expression");
    SWAG_ERROR(Err0466, "misplaced [[moveref]]                             $ unexpected [[moveref]] on type [[%s]] $ [[moveref]] should be used on a pointer or a reference");
    SWAG_ERROR(Err0467, "misplaced [[mtd]]                                 $ [[mtd]] is only valid within a struct implementation block $ you could replace [[mtd]] with [[func]]");
    SWAG_ERROR(Err0468, "misplaced [[mtd]] in enum                         $ [[mtd]] is only valid within a struct implementation block $ you could replace [[mtd]] with [[func]]");
    SWAG_ERROR(Err0469, "misplaced [[retval]]                              $ the [[retval]] special type can only be used within a function body");
    SWAG_ERROR(Err0470, "misplaced [[self]]                                $ the [[self]] keyword is only valid within an [[impl]] block of an enum or a struct");
    SWAG_ERROR(Err0471, "misplaced [[throw]]                               $ raising an error with [[throw]] is only valid within a function, macro or mixin");
    SWAG_ERROR(Err0472,
               "misplaced [[using]]                               $ [[using]] can't be associated with type [[%s]] $ expected a namespace, a struct, an enum or a variable");
    SWAG_ERROR(Err0473, "misplaced [[using]]                               $ [[using]] is not valid on a constant");
    SWAG_ERROR(Err0474, "misplaced [[using]]                               $ [[using]] is not valid on a variable with type [[%s]]");
    SWAG_ERROR(Err0475, "misplaced [[using]]                               $ [[using]] on a field is only valid for struct and pointer to struct, got [[%s]] instead");
    SWAG_ERROR(Err0476, "misplaced [[using]]                               $ [[using]] on a pointer variable is only valid for struct pointers, got a pointer to [[%s]] instead");
    SWAG_ERROR(Err0477, "misplaced [[using]]                               $ [[using]] on a variable is not allowed in [[%s]] scope");
    SWAG_ERROR(Err0478, "misplaced [[using]] in interface                  $ [[using]] can't be associated with an interface member");
    SWAG_ERROR(Err0479, "misplaced access modifier                         $ a global variable can't be [[public]]");
    SWAG_ERROR(Err0480, "misplaced access modifier                         $ misuse of [[%s]] before [[%s]] $ [[%s]] can't be prefixed with an access modifier");
    SWAG_ERROR(Err0481, "misplaced access modifier                         $ the [[%s]] access modifier can only be used at the global scope");
    SWAG_ERROR(Err0482, "misplaced array size                              $ the size of the array must be placed before the type name");
    SWAG_ERROR(Err0483, "misplaced attribute                               $ a [[switch]] without an expression can't be [[#[Swag.Complete]]]");
    SWAG_ERROR(Err0484, "misplaced attribute                               $ an interface member relocation with [[#[Swag.Offset]]] is not allowed");
    SWAG_ERROR(Err0485, "misplaced attribute                               $ incorrect attribute usage");
    SWAG_ERROR(Err0486, "misplaced attribute                               $ the %s can't have the [[#[Swag.Implicit]]] attribute because it is generic");
    SWAG_ERROR(Err0487,
               "misplaced attribute                               $ the [[#[Swag.CalleeReturn]]] attribute can't be applied to function [[%s]] $ [[#[Swag.CalledReturn]]] can only be applied to a macro ([[#[Swag.Macro]]]) or a mixin ([[#[Swag.Mixin]]])");
    SWAG_ERROR(Err0488,
               "misplaced attribute                               $ the [[#[Swag.Complete]]] attribute can't be applied to function [[%s]] $ #[Swag.Complete] can only be applied to [[opAffect]] and [[opAffectSuffix]]");
    SWAG_ERROR(Err0489, "misplaced attribute                               $ the [[#[Swag.Discardable]]] attribute can only be applied to lambda variables, got [[%s]]");
    SWAG_ERROR(Err0490,
               "misplaced attribute                               $ the [[#[Swag.Implicit]]] attribute can't be applied to function [[%s]] $ #[Swag.Implicit] can only be applied to [[opAffect]], [[opAffectSuffix]] and [[opCast]]");
    SWAG_ERROR(Err0491, "misplaced attribute                               $ the attribute [[%s]] can only be applied to %s");
    SWAG_ERROR(Err0492, "misplaced attribute                               $ the attribute [[%s]] can't be associated with %s");
    SWAG_ERROR(Err0493,
               "misplaced attribute                               $ the attribute [[%s]] can't be used in a [[#global]] statement $ the attribute does not have the [[Swag.AttributeUsage.File]] usage");
    SWAG_ERROR(Err0494,
               "misplaced attribute                               $ unexpected start of an attribute after [[%s]] $ attributes should be placed before access modifiers; consider moving the attribute before [[%s]]");
    SWAG_ERROR(Err0495, "misplaced attribute                               $ unexpected usage of the attribute [[%s]] in this context");
    SWAG_ERROR(Err0496,
               "misplaced closure                                 $ a closure definition is not valid at the global level $ closures can only be defined within functions");
    SWAG_ERROR(Err0497, "misplaced compiler directive                      $ [[#callerfunction]] can only be used when declaring a function parameter");
    SWAG_ERROR(Err0498, "misplaced compiler directive                      $ [[#callerlocation]] can only be used when declaring a function parameter");
    SWAG_ERROR(Err0499, "misplaced error catching                          $ [[%s]] can only be associated with function calls, but [[%s]] is %s");
    SWAG_ERROR(Err0500,
               "misplaced error catching                          $ [[%s]] can't be nested in a [[%s]] expression $ you should use only one [[try]], [[catch]] or [[assume]]");
    SWAG_ERROR(Err0501, "misplaced error catching                          $ treating a return error with [[%s]] can only be done inside a function, macro or mixin");
    SWAG_ERROR(Err0502, "misplaced error catching                          $ unexpected [[%s]] because the function [[%s]] can't raise an error");
    SWAG_ERROR(Err0503,
               "misplaced function declaration                    $ functions can't be directly declared inside structs $ functions associated with structs should be declared inside [[impl]] blocks");
    SWAG_ERROR(Err0504, "misplaced generic arguments                       $ unexpected generic arguments for %s [[%s]]");
    SWAG_ERROR(Err0505, "misplaced namespace                               $ a namespace should be defined in the top-level scope or within another namespace");
    SWAG_ERROR(Err0506, "misplaced range                                   $ can't use a range in a switch without an expression");
    SWAG_ERROR(Err0507, "misplaced reference to [[with]] block             $ statements starting with the [[.]] operator are only valid within a [[with]] block");
    SWAG_ERROR(Err0508,
               "misplaced special function                        $ the struct special function [[%s]] should be declared within the corresponding [[impl]] block $ an [[impl for]] block can only be used to implement interfaces");
    SWAG_ERROR(Err0509, "misplaced special function                        $ the struct special function [[%s]] should be declared within the corresponding [[impl]] block");
    SWAG_ERROR(Err0510, "misplaced top-level [[using]]                     $ a top-level [[using]] must be placed before any other declarations");
    SWAG_ERROR(Err0511, "misplaced tuple unpacking                         $ tuple unpacking is not allowed in %s");
    SWAG_ERROR(Err0512, "misplaced type                                    $ [[%s]] is only valid when declaring a function parameter");
    SWAG_ERROR(Err0513, "misplaced variadic parameter                      $ a variadic parameter must be the last one");
    SWAG_ERROR(Err0514, "misplaced variadic argument                       $ a variadic argument must be the last one");
    SWAG_ERROR(Err0515, "missing [[#alias]] number                         $ [[#alias]] variable names should end with a number such as [[#alias0]], [[#alias1]], etc.");
    SWAG_ERROR(Err0516, "missing [[#do]]                                   $ missing either [[#do]] or [[{]] before [[%s]]");
    SWAG_ERROR(Err0517,
               "missing [[#foreignlib]] string argument           $ expected the library name after [[#foreignlib]], found [[%s]] instead $ use [[#foreignlib \"path/to/library\"]] to specify the library to import");
    SWAG_ERROR(Err0518, "missing [[#global testerror]] string argument     $ expected the error string to check after [[#global testerror]], found [[%s]] instead");
    SWAG_ERROR(Err0519, "missing [[#global testwarning]] string argument   $ expected the warning string to check after [[#global testwarning]], found [[%s]] instead");
    SWAG_ERROR(Err0520, "missing [[#if]] boolean expression                $ expected a boolean expression before [[%s]]");
    SWAG_ERROR(Err0521, "missing [[#import]] string argument               $ expected the module name to import after [[#import]], found [[%s]] instead");
    SWAG_ERROR(Err0522, "missing [[#include]] string argument              $ expected the file name to include after [[#include]], found [[%s]] instead");
    SWAG_ERROR(Err0523, "missing [[#main]]                                 $ the program entry point [[#main]] is missing");
    SWAG_ERROR(Err0524, "missing [[#message]] parameter                    $ [[#message]] requests a parameter of type [[Swag.CompilerMsgMask]]");
    SWAG_ERROR(Err0525, "missing [[#mixin]] number                         $ [[#mixin]] variable names should end with a number such as [[#mixin0]], [[#mixin1]], etc.");
    SWAG_ERROR(Err0526, "missing [[#placeholder]] identifier               $ expected an identifier after [[#placeholder]], found [[%s]] instead");
    SWAG_ERROR(Err0527, "missing [[#type]] type                            $ expected a valid type after [[#type]], found [[%s]] instead");
    SWAG_ERROR(Err0528, "missing [[(]] before arguments                    $ expected [[(]] to start the list of arguments, found [[%s]] instead");
    SWAG_ERROR(Err0529, "missing [[(]] before parameters                   $ expected [[(]] to start the list of parameters, found [[%s]] instead");
    SWAG_ERROR(Err0530,
               "missing [[:]]                                     $ a [[:]] is expected here after the [[loop]] variable name [[%s]] $ syntax is [[loop [variable:] count]]");
    SWAG_ERROR(Err0531, "missing [[case]] expression                       $ expected the [[case]] expression before [[%s]]");
    SWAG_ERROR(Err0532,
               "missing [[closure]] capture variable              $ expected another capture variable between [[,]] and [[|]] $ add another variable or consider removing the trailing comma");
    SWAG_ERROR(Err0533, "missing [[do]]                                    $ missing either [[do]] or [[{]] before [[%s]]");
    SWAG_ERROR(Err0534, "missing [[if]] boolean expression                 $ expected a boolean expression before [[%s]]");
    SWAG_ERROR(Err0535, "missing [[impl]]                                  $ the function [[%s]] is part of the interface [[%s]] but lacks an [[impl]] marker");
    SWAG_ERROR(Err0536, "missing [[loop]] count                            $ expected the [[loop]] count expression before [[%s]]");
    SWAG_ERROR(Err0537, "missing [[visit]] expression                      $ expected the element to visit before [[%s]]");
    SWAG_ERROR(Err0538,
               "missing [[visit]] variable                        $ expected another [[visit]] variable between [[,]] and [[:]] $ the first [[visit]] variable is the value, the second one is the index");
    SWAG_ERROR(Err0539, "missing [[while]] boolean expression              $ expected a boolean expression before [[%s]]");
    SWAG_ERROR(Err0540, "missing arguments                                 $ the %s should have arguments but none were provided");
    SWAG_ERROR(Err0541, "missing arguments                                 $ the [[#[Swag.Using]]] attribute should have at least one argument");
    SWAG_ERROR(Err0542, "missing attribute                                 $ [[opVisit]] functions must have the [[#[Swag.Macro]]] attribute");
    SWAG_ERROR(Err0543, "missing attribute name                            $ expected an attribute name before [[(]]");
    SWAG_ERROR(Err0544, "missing catch error                               $ expected [[try]], [[assume]] or [[catch]] to deal with the errors of [[%s]]");
    SWAG_ERROR(Err0545, "missing closing [[%s]]                            $ expected [[%s]] %s, found [[%s]] instead");
    SWAG_ERROR(Err0546, "missing constant type                             $ expected a type with [[:]] or an assignment with [[=]], found [[%s]] instead");
    SWAG_ERROR(Err0547, "missing default value                             $ the %s needs a default value as a preceding parameter has one");
    SWAG_ERROR(Err0548,
               "missing dereference index                         $ an index is missing to dereference the array [[%s]] of type [[%s]] $ consider adding the index between brackets");
    SWAG_ERROR(Err0549,
               "missing dereference index                         $ an index is missing to dereference the slice [[%s]] of type [[%s]] $ consider adding the index between brackets");
    SWAG_ERROR(Err0550,
               "missing end of line                               $ expected a line break or a [[;]] to close the %s, found [[%s]] instead $ consider adding a line break or separating the previous statement with a [[;]]");
    SWAG_ERROR(Err0551, "missing enum flag value                           $ can't deduce the value of flag [[%s]] as the previous value isn't a power of two");
    SWAG_ERROR(Err0552, "missing enum name                                 $ expected the enum name before the start of the body [[{]]");
    SWAG_ERROR(Err0553, "missing enum name                                 $ expected the enum name before the start of the type definition [[:]]");
    SWAG_ERROR(Err0554,
               "missing function parameters                       $ expected the function parameters between [[()]] before the start of the body [[{]] $ if you want to declare a function without parameters, consider adding [[()]] after the name");
    SWAG_ERROR(Err0555, "missing generic arguments                         $ can't instantiate %s because of missing generic arguments");
    SWAG_ERROR(Err0556, "missing generic arguments                         $ can't instantiate the generic function [[%s]] because of missing generic arguments");
    SWAG_ERROR(Err0557, "missing generic arguments                         $ can't instantiate the generic struct [[%s]] because of missing generic arguments");
    SWAG_ERROR(Err0558,
               "missing generic parameters                        $ expected at least one generic parameter between [[()]] $ consider removing [[()]] if this is not generic");
    SWAG_ERROR(Err0559, "missing generic parameters                        $ the special function [[%s]] expects some generic parameters");
    SWAG_ERROR(Err0560, "missing generic parameters                        $ the struct [[%s]] can't be solved due to the generic field [[%s]]");
    SWAG_ERROR(Err0561, "missing generic parameters                        $ the struct [[%s]] should declare generic parameters because [[%s]] is generic");
    SWAG_ERROR(Err0562, "missing initialization                            $ a constant requires an explicit initialization");
    SWAG_ERROR(Err0563, "missing initialization                            $ a reference requires an explicit initialization");
    SWAG_ERROR(Err0564, "missing initialization                            $ a variable declared with [[let]] requires an explicit initialization");
    SWAG_ERROR(Err0565, "missing initialization                            $ an immutable variable requires an explicit initialization");
    SWAG_ERROR(Err0566, "missing initialization                            $ expected an initialization of [[%s]] because [[%s]] does not contain a value for zero");
    SWAG_ERROR(Err0567, "missing initialization                            $ the enumeration value [[%s]] has type [[%s]] which requires an explicit initialization");
    SWAG_ERROR(Err0568, "missing intrinsic arguments                       $ expected the intrinsic arguments between [[()]]");
    SWAG_ERROR(Err0569,
               "missing lambda parameter type                     $ expected the type with [[:]] or an assignment with [[=]] $ when declaring lambda parameters, you should not combined inferred and explicit types");
    SWAG_ERROR(Err0570, "missing named argument                            $ the %s should be named because some arguments before are named");
    SWAG_ERROR(Err0571, "missing parameter type                            $ expected the type with [[:]] or an assignment with [[=]], found [[%s]] instead");
    SWAG_ERROR(Err0572, "missing parameters                                $ the special function [[%s]] requires some parameters");
    SWAG_ERROR(Err0573, "missing return type                               $ the function [[%s]] has the [[#[Swag.Discardable]]] attribute and should return something");
    SWAG_ERROR(Err0574, "missing return type                               $ the special function [[%s]] requires a return type");
    SWAG_ERROR(Err0575, "missing return type                               $ the special function [[%s]] should return a type [[%s]]");
    SWAG_ERROR(Err0576, "missing return value                              $ expected a return value because the return type has already been deduced to be [[%s]]");
    SWAG_ERROR(Err0577, "missing return value                              $ expected an expression of type [[%s]] after [[return]]");
    SWAG_ERROR(Err0578, "missing return value                              $ not all control paths of %s returns a value");
    SWAG_ERROR(Err0579, "missing return value                              $ the %s returns a type [[%s]] but lacks a [[return]] in its body");
    SWAG_ERROR(Err0580,
               "missing slicing upper bound                       $ expected an upper bound expression after [[..<]] $ if you want to slice to the end, consider using [[..]] instead of [[..>]]");
    SWAG_ERROR(Err0581, "missing struct name                               $ expected the struct name before the start of the body [[{]]");
    SWAG_ERROR(Err0582, "missing test error                                $ expected at least one error, but none was raised");
    SWAG_ERROR(Err0583, "missing test warning                              $ expected at least one warning, but none was raised");
    SWAG_ERROR(Err0584, "missing variable type                             $ expected the type with [[:]] or an assignment with [[=]], found [[%s]] instead");
    SWAG_ERROR(Err0585, "misused UFCS                                      $ the %s [[%s]] is not used as the first argument when calling [[%s]]");
    SWAG_ERROR(Err0586, "misused UFCS                                      $ the hidden [[with]] variable [[%s]] is not used as the first argument when calling [[%s]]");
    SWAG_ERROR(Err0587, "non-contiguous [[#alias]]                         $ non-contiguous [[#alias]] in function [[%s]], missing [[#alias%u]]");
    SWAG_ERROR(Err0588, "non-transferable [[cvarargs]] type                $ a parameter with the type [[cvarargs]] can't be passed to another function");
    SWAG_ERROR(Err0589, "not a value                                       $ %s [[%s]] is not a valid value in this context");
    SWAG_ERROR(Err0590, "not a value                                       $ implicit reference to a static struct member of [[%s]]");
    SWAG_ERROR(Err0591, "not enough arguments                              $ not enough arguments for %s");
    SWAG_ERROR(Err0592, "not enough arguments                              $ not enough arguments to call %s");
    SWAG_ERROR(Err0593, "not enough generic arguments                      $ not enough generic arguments for %s");
    SWAG_ERROR(Err0594, "not enough generic parameters                     $ not enough generic parameters for special function [[%s]]");
    SWAG_ERROR(Err0595, "not enough initializers                           $ expected [[%d]] value(s) to initialize the array, got [[%d]] instead");
    SWAG_ERROR(Err0596, "not enough initializers                           $ expected [[%d]] value(s) to initialize the tuple, got [[%d]] instead");
    SWAG_ERROR(Err0597, "not enough parameters                             $ not enough parameters for special function [[%s]] (expected [[%d]], got [[%d]])");
    SWAG_ERROR(Err0598, "not enough parameters                             $ too few parameters for special function [[%s]] (at least [[%d]] expected, only [[%d]] provided)");
    SWAG_ERROR(Err0599, "null dereference                                  $ attempt to dereference a null pointer");
    SWAG_ERROR(Err0600, "out of range [[#alias]] number                    $ an [[#alias]] number should be in the range [0, 31], found [[%u]]");
    SWAG_ERROR(Err0601, "out of range [[#mixin]] number                    $ a [[#mixin]] number should be in the range [0, 31], found [[%u]]");
    SWAG_ERROR(Err0602, "out of range [[#up]] count                        $ the [[#up]] count should be in the range [1, 255], found [[%u]]");
    SWAG_ERROR(Err0603, "out of range array dimensions                     $ array dimensions can't exceed [[254]]");
    SWAG_ERROR(Err0604, "out of range call level                           $ the limit defined with [[--limit-recurse-bc:%d]] has been reached");
    SWAG_ERROR(Err0605, "out of range inline level                         $ can't expand [[%s]] because the limit defined with [[--limit-inline:%d]] has been reached");
    SWAG_ERROR(Err0606, "out of range character literal                    $ can't convert the character literal [[0x%x]] to [[u16]], this is out of range");
    SWAG_ERROR(Err0607, "out of range character literal                    $ can't convert the character literal [[0x%x]] to [[u8]], this is out of range");
    SWAG_ERROR(Err0608, "out of range enum value                           $ the enum value [[%s]] exceeds the valid range of type [[%s]]");
    SWAG_ERROR(Err0609, "out of range number                               $ this literal number exceeds 64 bits and is out of allowable range");
    SWAG_ERROR(Err0610, "out of range slicing                              $ the slicing bound [[%I64u]] is out of range (max is [[%I64u]])");
    SWAG_ERROR(Err0611, "out of range slicing                              $ the slicing lower bound [[%I64u]] is greater than the upper bound [[%I64u]]");
    SWAG_ERROR(Err0612, "out of range slicing                              $ the slicing upper bound exclusion with [[..<]] is not possible because it is zero");
    SWAG_ERROR(Err0613, "overload mismatch                                 $ found [[%d]] overloads for [[%s]], but none matched the arguments");
    SWAG_ERROR(Err0614, "recursive evaluation                              $ the struct [[%s]] references itself, which is causing a recursion");
    SWAG_ERROR(Err0615,
               "redundant access modifier                         $ the [[%s]] access modifier is unnecessary because of [[#global export]] $ consider removing the [[public]] modifier, as [[#global export]] ensures all declarations in this file are publicly accessible");
    SWAG_ERROR(Err0616, "reserved function name                            $ the function name [[%s]] is reserved by the compiler");
    SWAG_ERROR(Err0617,
               "reserved identifier                               $ the identifier [[%s]] starts with [[__]] $ identifiers starting with [[__]] are typically reserved for compiler-internal or system use");
    SWAG_ERROR(Err0618, "reserved namespace name                           $ the namespace [[%s]] is reserved by the compiler");
    SWAG_ERROR(Err0619,
               "reserved struct member name                       $ the struct member name [[%s]] is reserved by the compiler $ struct member names beginning with [[item]] are reserved by the language");
    SWAG_ERROR(Err0620, "return type mismatch                              $ an [[#ast]] block must return a string, got [[%s]]");
    SWAG_ERROR(Err0621, "return type mismatch                              $ the return type has already been deduced to be [[%s]], and here it's [[%s]]");
    SWAG_ERROR(Err0622, "return type mismatch                              $ the return type has already been deduced to be nothing, and here it's [[%s]]");
    SWAG_ERROR(Err0623, "return type mismatch                              $ unexpected return type [[%s]] for function [[%s]] $ consider adding a return type [[->%s]]");
    SWAG_ERROR(Err0624, "semantic cycle                                    $ a cycle has been detected during the resolution of %s [[%s]]");
    SWAG_ERROR(Err0625, "standalone expression                             $ an expression value should be used $ consider removing it");
    SWAG_ERROR(Err0626, "symbol already defined                            $ the %s [[%s]] has already been defined %s");
    SWAG_ERROR(Err0627, "symbol already defined                            $ the %s [[%s]] has already been defined in a parent scope");
    SWAG_ERROR(Err0628, "system error                                      $ failed to create the process [[%s]] (::CreatePipe)");
    SWAG_ERROR(Err0629, "system error                                      $ failed to create the process [[%s]]");
    SWAG_ERROR(Err0630, "too many arguments                                $ expected [[%d]] argument(s), got [[%d]] instead");
    SWAG_ERROR(Err0631, "too many generic arguments                        $ expected [[%d]] generic argument(s) for %s [[%s]], got [[%d]] instead");
    SWAG_ERROR(Err0632, "too many generic parameters                       $ expected one single generic parameter for special function [[%s]], got [[%d]] instead");
    SWAG_ERROR(Err0633, "too many generic parameters                       $ too many generic parameters for special function [[%s]]");
    SWAG_ERROR(Err0634, "too many initializers                             $ expected [[%d]] value(s) to initialize [[%s]], got [[%d]] instead");
    SWAG_ERROR(Err0635, "too many initializers                             $ expected [[%d]] value(s) to initialize the array, got [[%d]] instead");
    SWAG_ERROR(Err0636, "too many initializers                             $ expected [[%d]] value(s) to initialize the tuple, got [[%d]] instead");
    SWAG_ERROR(Err0637, "too many initializers                             $ too many initializers for type [[%s]]");
    SWAG_ERROR(Err0638, "too many parameters                               $ too many parameters for [[%s]] ([[%d]] expected, [[%d]] provided)");
    SWAG_ERROR(Err0639, "too many variadic parameters                      $ the maximum number of variadic parameters is [[%s]], got [[%d]] instead");
    SWAG_ERROR(Err0640, "tuple type mismatch                               $ the source and the requested tuples are not compatible");
    SWAG_ERROR(Err0641, "type mismatch                                     $ a closure can't be assigned to a lambda type");
    SWAG_ERROR(Err0642, "type mismatch                                     $ can't initialize type [[%s]] from type [[%s]]");
    SWAG_ERROR(Err0643, "type mismatch                                     $ casting from [[%s]] to [[%s]] is not allowed");
    SWAG_ERROR(Err0644, "type mismatch                                     $ casting from [[%s]] to pointer type is not allowed $ only conversion from type [[u64]] is accepted");
    SWAG_ERROR(Err0645, "type mismatch                                     $ casting from a type value ([[typeinfo]]) to a compile type ([[%s]]) is not allowed");
    SWAG_ERROR(Err0646, "type mismatch                                     $ casting from a value pointer [[%s]] to a block pointer [[%s]] is not allowed");
    SWAG_ERROR(Err0647, "type mismatch                                     $ casting from closure to lambda is not allowed");
    SWAG_ERROR(Err0648, "type mismatch                                     $ expected [[%s]] for the UFCS argument, got [[%s]] instead");
    SWAG_ERROR(Err0649, "type mismatch                                     $ expected type [[%s]], got [[%s]] instead");
    SWAG_ERROR(Err0650, "type mismatch                                     $ expected type [[*Swag.CVaList]], got [[%s]] instead");
    SWAG_ERROR(Err0651, "type mismatch                                     $ the [[%s]] assignment requires an expression of type [[%s]], got [[%s]] instead");
    SWAG_ERROR(Err0652, "type mismatch                                     $ type [[%s]] and type [[%s]] defined in the command line for [[%s]] are incompatible");
    SWAG_ERROR(Err0653, "type mismatch                                     $ expected [[%s]] for field [[%s]], got [[%s]] instead");
    SWAG_ERROR(Err0654, "type mismatch                                     $ expected return type [[%s]] for special function [[%s]], got [[%s]] instead");
    SWAG_ERROR(Err0655,
               "unauthorized function overload                    $ a function with the same name [[%s]] has already been declared $ if you want to declare an overload, mark all functions with [[#[Swag.Overload]]]");
    SWAG_ERROR(Err0656, "unexpected [[#elif]]                              $ [[#elif]] found without a preceding [[#if]] directive");
    SWAG_ERROR(Err0657, "unexpected [[#else]]                              $ [[#else]] found without a preceding [[#if]] or [[#elif]] directive");
    SWAG_ERROR(Err0658,
               "unexpected [[#validif/#validifx]]                 $ the [[#validif/#validifx]] directive is not allowed on special function [[%s]] $ [[#validif]] and [[#validifx]] are not valid for [[opDrop]], [[opPostCopy]], and [[opPostMove]]");
    SWAG_ERROR(Err0659,
               "unexpected [[#validifx]] in struct                $ [[#validifx]] is not allowed for a struct, this is only valid for functions $ consider using [[#validif]] instead");
    SWAG_ERROR(Err0660, "unexpected [[)]]                                  $ symbol [[)]] found without a preceding [[(]]");
    SWAG_ERROR(Err0661, "unexpected [[]]]                                  $ symbol [[]]] found without a preceding [[[]]");
    SWAG_ERROR(Err0662, "unexpected [[const]]                              $ unexpected [[const]] before a lambda parameter name");
    SWAG_ERROR(Err0663, "unexpected [[default]]                            $ a [[switch]] marked with the [[#[Swag.Complete]]] attribute can't have a [[default]] statement");
    SWAG_ERROR(Err0664, "unexpected [[elif]]                               $ [[elif]] found without a preceding [[if]]");
    SWAG_ERROR(Err0665, "unexpected [[else]]                               $ [[else]] found without a preceding [[if]] or [[elif]]");
    SWAG_ERROR(Err0666, "unexpected [[for]]                                $ the [[for]] keyword is not valid in an enum implementation");
    SWAG_ERROR(Err0667, "unexpected [[impl]]                               $ [[impl]] associated with a function should only be used within an [[impl for]] block");
    SWAG_ERROR(Err0668, "unexpected [[mtd]]                                $ expected [[func]] or [[closure]] instead of [[mtd]] to declare a lambda type");
    SWAG_ERROR(Err0669, "unexpected [[return]]                             $ [[return]] is implicit after [[=>]] $ consider removing it");
    SWAG_ERROR(Err0670, "unexpected [[retval]] return type                 $ [[retval]] is only valid as a local variable type");
    SWAG_ERROR(Err0671, "unexpected [[throw]] qualifier                    $ an attribute can't have the [[throw]] qualifier as it can't raise errors");
    SWAG_ERROR(Err0672,
               "unexpected [[var]] in struct                      $ [[var]] is unnecessary in struct variable declarations $ struct variables should be declared as [[fieldName: Type]] without [[var]]");
    SWAG_ERROR(Err0673, "unexpected [[}]]                                  $ symbol [[}]] found without a preceding [[{]]");
    SWAG_ERROR(Err0674, "unexpected affectation                            $ unexpected usage of the assignment operator [[=]] inside an expression");
    SWAG_ERROR(Err0675, "unexpected aliased names                          $ unexpected alias when calling [[%s]] because it's not a macro or a mixin");
    SWAG_ERROR(Err0676, "unexpected attribute return type                  $ an attribute can't have a return type starting with [[->]]");
    SWAG_ERROR(Err0677, "unexpected comparison                             $ expected an affectation with [[=]] but found the comparison operator [[==]] instead");
    SWAG_ERROR(Err0678, "unexpected default value                          $ default value for variadic parameters are not allowed");
    SWAG_ERROR(Err0679, "unexpected empty expression                       $ the expression [[()]] is empty and not allowed here");
    SWAG_ERROR(Err0680,
               "unexpected end of comment                         $ unexpected end of multi-line comment [[*/]] after %s $ beginning [[/*]] of multi-line comment is missing");
    SWAG_ERROR(Err0681, "unexpected end of file within comment             $ this multi-line comment is missing its closing [[*/]]");
    SWAG_ERROR(Err0682, "unexpected function body                          $ a function tagged with the [[#[Swag.Foreign]]] attribute can't have a body");
    SWAG_ERROR(Err0683, "unexpected generic arguments                      $ unexpected generic arguments after %s");
    SWAG_ERROR(Err0684, "unexpected generic function                       $ the function [[%s]] appears to be generic despite the [[#[Swag.NotGeneric]]] attribute");
    SWAG_ERROR(Err0685, "unexpected generic parameters                     $ a function interface can't have generic parameters");
    SWAG_ERROR(Err0686, "unexpected generic arguments                      $ expected an non-generic name without generic parameters");
    SWAG_ERROR(Err0687, "unexpected generic parameters                     $ the function [[%s]] can't have generic parameters because of the [[#[Swag.NotGeneric]]] attribute");
    SWAG_ERROR(Err0688, "unexpected generic parameters                     $ the special function [[%s]] can't have generic parameters");
    SWAG_ERROR(Err0689, "unexpected identifier                             $ a global identifier like [[%s]] can't be used at the file level");
    SWAG_ERROR(Err0690, "unexpected lambda parameter name                  $ unexpected name when declaring a lambda type $ consider removing the name");
    SWAG_ERROR(Err0691,
               "unexpected lambda parameter type                  $ unexpected type $ when declaring lambda parameters, you should not combined inferred and explicit types");
    SWAG_ERROR(Err0692, "unexpected line break in array type declaration   $ the type of the array should be placed on the same line as the array dimensions");
    SWAG_ERROR(Err0693, "unexpected name                                   $ too many [[visit]] alias names (maximum is [[2]], but [[%u]] are provided)");
    SWAG_ERROR(Err0694, "unexpected operator modifier                      $ the operator modifier [[%s]] can't be applied to [[%s]]");
    SWAG_ERROR(Err0695, "unexpected parameters                             $ expected [[{]] here because a compiler %s should not have parameters");
    SWAG_ERROR(Err0696, "unexpected return value                           $ a %s can't return something");
    SWAG_ERROR(Err0697, "unexpected return value                           $ a function with the [[#[Swag.CalleeReturn]]] attribute must not declare a return value");
    SWAG_ERROR(Err0698, "unexpected tuple unpacking name                   $ expected a variable name or [[?]] when unpacking a tuple, found [[%s]] instead");
    SWAG_ERROR(Err0699, "unexpected type                                   $ can't start a declaration with a type");
    SWAG_ERROR(Err0700, "unexpected type alias                             $ expected a direct type as an [[impl]] block name, got a type alias instead");
    SWAG_ERROR(Err0701,
               "unexpected type declaration                       $ unexpected type declaration after [[self]] $ [[self]] is implicitly typed; consider removing the redundant type");
    SWAG_ERROR(Err0702, "unexpected type declaration                       $ unexpected type declaration after unnamed parameters");
    SWAG_ERROR(Err0703, "unexpected type suffix                            $ %s should not be followed by a type suffix");
    SWAG_ERROR(Err0704, "unexpected visit name                             $ a special visit name is only valid to visit a struct, got [[%s]] instead");
    SWAG_ERROR(Err0705, "unknown attribute                                 $ the attribute [[%s]] can't be found");
    SWAG_ERROR(Err0706, "unknown compiler instruction                      $ the compiler instruction [[%s]] can't be found");
    SWAG_ERROR(Err0707, "unknown dependency                                $ use of an unknown module dependency [[%s]]");
    SWAG_ERROR(Err0708, "unknown enum value                                $ the enumeration value [[%s]] can't be found in [[%s]]");
    SWAG_ERROR(Err0709, "unknown file                                      $ the [[#load]] file [[%s]] can't be located or accessed");
    SWAG_ERROR(Err0710, "unknown file                                      $ the file named [[%s]] can't be located or accessed");
    SWAG_ERROR(Err0711, "unknown foreign function                          $ the foreign function [[%s]] was not found");
    SWAG_ERROR(Err0712, "unknown function                                  $ the function [[%s]] can't be found");
    SWAG_ERROR(Err0713, "unknown identifier                                $ the identifier [[%s]] can't be found in %s [[%s]]");
    SWAG_ERROR(Err0714, "unknown identifier                                $ the identifier [[%s]] can't be found in [[%s]] or in %s [[%s]]");
    SWAG_ERROR(Err0715, "unknown identifier                                $ the identifier [[%s]] can't be found in [[%s]]");
    SWAG_ERROR(Err0716, "unknown identifier                                $ the identifier [[%s]] can't be found in the tuple");
    SWAG_ERROR(Err0717, "unknown identifier                                $ the identifier [[%s]] can't be found");
    SWAG_ERROR(Err0718,
               "unknown identifier                                $ the identifier [[%s]] is preceded with [[.]] but no corresponding [[enum]] or [[with]] can be deduced");
    SWAG_ERROR(Err0719, "unknown identifier                                $ the variable [[%s]] from the struct [[%s]] has no corresponding member");
    SWAG_ERROR(Err0720, "unknown interface function                        $ the function [[%s]] is not part of the interface [[%s]]");
    SWAG_ERROR(Err0721, "unknown intrinsic                                 $ the intrinsic [[%s]] can't be found");
    SWAG_ERROR(Err0722, "unknown label                                     $ the scope label [[%s]] can't be found");
    SWAG_ERROR(Err0723, "unknown method                                    $ the method [[%s]] can't be found in [[%s]]");
    SWAG_ERROR(Err0724, "unknown parameter                                 $ the named parameter [[%s]] can't be found");
    SWAG_ERROR(Err0725, "unknown relocation offset                         $ can't find the struct member [[%s]] to compute the relocation");
    SWAG_ERROR(Err0726, "unknown struct                                    $ the struct [[%s]] can't be found");
    SWAG_ERROR(Err0727, "unknown type                                      $ the type [[%s]] can't be found");
    SWAG_ERROR(Err0728, "unpacking number mismatch                         $ trying to unpack [[%u]] variable(s) but the tuple has [[%u]] field(s)");
    SWAG_ERROR(Err0729, "unpacking number mismatch                         $ trying to unpack [[%u]] variable(s) but the tuple has only [[%u]] field(s)");
    SWAG_ERROR(Err0730, "unresolved identifier                             $ the identifier [[%s]] can't be solved");
    SWAG_ERROR(Err0731, "unresolved interface                              $ the interface can't be solved due to the generic type [[%s]]");
    SWAG_ERROR(Err0732, "unsuitable struct for union                       $ the struct [[%s]] is not suitable for union because it contains an [[%s]] function");
    SWAG_ERROR(Err0733, "unsupported [[@nameof]] argument                  $ can't evaluate the [[@nameof]] argument");
    SWAG_ERROR(Err0734, "unsupported [[@stringof]] argument                $ can't evaluate the [[@stringof]] argument");
    SWAG_ERROR(Err0735, "unknown field name                                $ the field name [[%s]] can't be found in [[%s]]");
    SWAG_ERROR(Err0736, "unsupported array type                            $ can't declare a static array of type [[void]]");
    SWAG_ERROR(Err0737, "unsupported callback                              $ the intrinsic [[@mkcallback]] does not allow a function returning [[%s]]");
    SWAG_ERROR(Err0738,
               "unsupported callback                              $ the intrinsic [[@mkcallback]] does not allow a function with more than [[%d]] parameters, found [[%d]]");
    SWAG_ERROR(Err0739, "unsupported generic                               $ unsupported generic type deduction, this is too complicated (embedded tuples)");
    SWAG_ERROR(Err0740, "unsupported index type                            $ the array access index must be an integer, got [[%s]] instead");
    SWAG_ERROR(Err0741, "unsupported tuple type                            $ tuple types are not supported for generic parameters");
    SWAG_ERROR(Err0742, "unsupported type                                  $ the intrinsic [[@countof]] does not accept a tuple as an argument");
    SWAG_ERROR(Err0743, "unsupported type                                  $ the intrinsic [[@dataof]] does not accept an argument of type [[%s]]");
    SWAG_ERROR(Err0744, "unsupported type                                  $ the intrinsic [[@dataof]] does not accept a tuple as an argument");
    SWAG_ERROR(Err0745, "unsupported variadic parameters                   $ an inline function can't have variadic parameters");
    SWAG_ERROR(Err0746, "unused alias name                                 $ the alias name [[%s]] has no corresponding variable $ consider removing it");
    SWAG_ERROR(Err0747,
               "unused return value                               $ the return value of the function [[%s]] should be used $ if you don't need the return value, consider prefixing the call with [[discard]]");
    SWAG_ERROR(Err0748, "unused return value                               $ the return value of the intrinsic [[%s]] should be used");
    SWAG_ERROR(Err0749,
               "unused return value                               $ the return value of the lambda [[%s]] should be used $ if you don't need the return value, consider prefixing the call with [[discard]]");
    SWAG_ERROR(Err0750, nullptr);
    SWAG_ERROR(Err0751, nullptr);
    SWAG_ERROR(Err0752, nullptr);
    SWAG_ERROR(Err0753, nullptr);
    SWAG_ERROR(Err0754, nullptr);
    SWAG_ERROR(Err0755, nullptr);
    SWAG_ERROR(Err0756, nullptr);
    SWAG_ERROR(Err0757, nullptr);
    SWAG_ERROR(Err0758, nullptr);
    SWAG_ERROR(Err0759, nullptr);

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    SWAG_ERROR(Nte0001, "[[#alias]] is only valid within a [[#[Swag.Macro]]] or [[#[Swag.Mixin]]] function");
    SWAG_ERROR(Nte0002, "[[#mixin]] is only valid within a [[#[Swag.Mixin]]] function");
    SWAG_ERROR(Nte0003, "[[%s]] is %s and does not have a sub-scope");
    SWAG_ERROR(Nte0004, "[[%s]] is %s of type [[%s]] and does not have a sub-scope");
    SWAG_ERROR(Nte0005, "[[%s]] is a variable and variables within expressions can't be evaluated at compile-time");
    SWAG_ERROR(Nte0006, "[[%s]] is an array of type [[%s]] which does not have a sub-scope");
    SWAG_ERROR(Nte0007, "[[%s]] is needed because of the function return type");
    SWAG_ERROR(Nte0008, "[[%s]] might represent either a type or a parameter name");
    SWAG_ERROR(Nte0009, "[[%s]] was located within [[%s]] due to a [[using]] field");
    SWAG_ERROR(Nte0010, "[[==]] is the comparison operator; did you intend to affect something with [[=]] ?");
    SWAG_ERROR(Nte0011, "[[closure]] should be followed by the capture parameters enclosed in [[|...|]]");
    SWAG_ERROR(Nte0012, "[[discard]] can't be associated with an intrinsic, as an intrinsic result should always be used");
    SWAG_ERROR(Nte0013, "[[namealias]] should be associated with a namespace, a function or a variable instead of a type");
    SWAG_ERROR(Nte0014, "[[with]] should be followed by a single identifier");
    SWAG_ERROR(Nte0015, "a [[%s]] block must start with [[#do]] or must be enclosed in [[{}]]");
    SWAG_ERROR(Nte0016, "a [[%s]] block must start with [[do]] or must be enclosed in [[{}]]");
    SWAG_ERROR(Nte0017, "a [[let]] variable is immutable and can't be changed");
    SWAG_ERROR(Nte0018, "a return type is missing");
    SWAG_ERROR(Nte0019, "a standard function in an [[impl]] block should not shadow a function from the matching interface");
    SWAG_ERROR(Nte0020, "an attribute is not a function; consider removing it");
    SWAG_ERROR(Nte0021, "an implicit [[@countof]] is present here");
    SWAG_ERROR(Nte0022, "associated command line option: [[%s]]");
    SWAG_ERROR(Nte0023, "both parts of an [[orelse]] should be of identical type");
    SWAG_ERROR(Nte0024, "but it is applied on this %s");
    SWAG_ERROR(Nte0025, "consider [[typealias]] to create an alias for %s");
    SWAG_ERROR(Nte0026, "consider adding [[&]] to get the address of this expression");
    SWAG_ERROR(Nte0027, "consider adding [[using]] before [[self]] if you intend to access members of the instance directly");
    SWAG_ERROR(Nte0028, "consider adding a [[break]] if you want to exit without any action");
    SWAG_ERROR(Nte0029, "consider adding a [[break]] to exit, or use [[fallthrough]] to continue to the next [[case]]");
    SWAG_ERROR(Nte0030, "consider adding an explicit [[cast(%s)]] if necessary");
    SWAG_ERROR(Nte0031, "consider adding scope [[%s]] before [[.]]");
    SWAG_ERROR(Nte0032, "consider casting to the underlying [[any]] type");
    SWAG_ERROR(Nte0033, "consider casting with [[cast(%s)]]");
    SWAG_ERROR(Nte0034, "consider removing one [[using]]");
    SWAG_ERROR(Nte0035, "consider removing the %s or replace it with the scope [[%s]]");
    SWAG_ERROR(Nte0036, "consider replacing [[undefined]] with an explicit initialization");
    SWAG_ERROR(Nte0037, "consider using [[++]] to append literal strings at compile-time");
    SWAG_ERROR(Nte0038, "consider using [[?]] to unpack and ignore a variable");
    SWAG_ERROR(Nte0039, "consider using [[func]] instead of [[mtd]]");
    SWAG_ERROR(Nte0040, "consider using [[func]] to declare a function");
    SWAG_ERROR(Nte0041, "consider using [[mtd]] instead of [[func]] to implicitly declare [[using self]] as the first parameter");
    SWAG_ERROR(Nte0042, "consider writing [[impl %s]] instead");
    SWAG_ERROR(Nte0043, "consider writing [[impl enum %s]] instead");
    SWAG_ERROR(Nte0044, "copying an array of structs is not allowed here");
    SWAG_ERROR(Nte0045, "could [[self]] be missing?");
    SWAG_ERROR(Nte0046, "could be %s of type [[%s]]");
    SWAG_ERROR(Nte0047, "could be %s");
    SWAG_ERROR(Nte0048, "could be a function of type [[%s]]");
    SWAG_ERROR(Nte0049, "could be an instance of the generic function [[%s]]");
    SWAG_ERROR(Nte0050, "could be the struct [[%s]]");
    SWAG_ERROR(Nte0051, "could be");
    SWAG_ERROR(Nte0052, "detected an enum implementation block");
    SWAG_ERROR(Nte0053, "did you forget [[var]] or [[const]] to declare a global variable or a constant?");
    SWAG_ERROR(Nte0054, "employ [[{}]] for an intentional empty statement");
    SWAG_ERROR(Nte0055, "entity %s [[%s]] awaits the generation of type [[%s]]");
    SWAG_ERROR(Nte0056, "evaluation failed during compile-time");
    SWAG_ERROR(Nte0057, nullptr);
    SWAG_ERROR(Nte0058, "function names that start with [[op]] followed by an uppercase letter are reserved for struct special functions");
    SWAG_ERROR(Nte0059, "function parameters are immutable and can't be modified");
    SWAG_ERROR(Nte0060, "here is another one");
    SWAG_ERROR(Nte0061, "here is one");
    SWAG_ERROR(Nte0062, "here is the %s [[%s]]");
    SWAG_ERROR(Nte0063, "here is the %s");
    SWAG_ERROR(Nte0064, "here is the corresponding parameter [[%s]]");
    SWAG_ERROR(Nte0065, "here is the corresponding parameter");
    SWAG_ERROR(Nte0066, "here is the deprecated definition");
    SWAG_ERROR(Nte0067, "here is the field causing the recursion");
    SWAG_ERROR(Nte0068, "here is the generic parameter [[%s]] of the %s");
    SWAG_ERROR(Nte0069, "here is the origin of the instantiation type [[%s]]");
    SWAG_ERROR(Nte0070, "here is the other [[#import]]");
    SWAG_ERROR(Nte0071, "here is the other definition");
    SWAG_ERROR(Nte0072, "here is the other return statement");
    SWAG_ERROR(Nte0073, "here is the previous definition");
    SWAG_ERROR(Nte0074, "here is the prior declaration");
    SWAG_ERROR(Nte0075, nullptr);
    SWAG_ERROR(Nte0076, "here is the reference to the immutable value");
    SWAG_ERROR(Nte0077, "here is the runtime argument corresponding to [[%s]]");
    SWAG_ERROR(Nte0078, "here is the tuple definition");
    SWAG_ERROR(Nte0079, "if needed, you can prefix with [[#run]] to force a compile-time call");
    SWAG_ERROR(Nte0080, "if this is intended, consider adding [[impl]] before this");
    SWAG_ERROR(Nte0081, "if this is intended, consider initializing the global variable with [[undefined]] instead of zero");
    SWAG_ERROR(Nte0082, "if this is intended, consider renaming it to [[_%s]] to indicate it's unused");
    SWAG_ERROR(Nte0083, "if you want to declare a generic constant, consider adding [[const]] before [[%s]]");
    SWAG_ERROR(Nte0084, "if you want to declare a lambda type, use [[func(]] or [[closure(]]");
    SWAG_ERROR(Nte0085, "if you want to retrieve the type of an expression, consider using [[@decltype]] instead")
    SWAG_ERROR(Nte0086, "if you were trying to compare values, you should use [[==]] instead");
    SWAG_ERROR(Nte0087, "internal structs should not export their special functions");
    SWAG_ERROR(Nte0088, "it seems like you're trying to access a nested property of [[%s]], but [[%s]] itself isn't a value");
    SWAG_ERROR(Nte0089, "missing %s of type [[%s]]");
    SWAG_ERROR(Nte0090, "missing parameter [[%s]] of type [[%s]]");
    SWAG_ERROR(Nte0091, "occurred during compile-time evaluation");
    SWAG_ERROR(Nte0092, "occurred during the [[#validif]] check of struct [[%s]]");
    SWAG_ERROR(Nte0093, "occurred during the [[#validif]] check of the call to [[%s]]");
    SWAG_ERROR(Nte0094, "occurred during the [[#validifx]] check of the call to [[%s]]");
    SWAG_ERROR(Nte0095, "occurred during the generic instantiation of [[%s]]");
    SWAG_ERROR(Nte0096, "occurred during the inline expansion of [[%s]]");
    SWAG_ERROR(Nte0097, "occurred during the public export of [[%s]]");
    SWAG_ERROR(Nte0098, "occurred in generated code");
    SWAG_ERROR(Nte0099, "one function declares [[throw]] while the other doesn't");
    SWAG_ERROR(Nte0100, "only the address of a returned reference can be taken, and this is %s");
    SWAG_ERROR(Nte0101, "only variables can be initialized that way");
    SWAG_ERROR(Nte0102, "parameter mismatch (type is [[%s]])");
    SWAG_ERROR(Nte0103, "pointer arithmetic is only valid for pointers declared with [[^]], not [[*]]");
    SWAG_ERROR(Nte0104, "possibly comes from [[%s]]");
    SWAG_ERROR(Nte0105, "potential issue detected in your program's compile-time component");
    SWAG_ERROR(Nte0106, "public structs should export all their special functions");
    SWAG_ERROR(Nte0107, "should be casted to a sized integer like [[s32]], [[s64]], etc.");
    SWAG_ERROR(Nte0108, "should conform to type [[%s]]");
    SWAG_ERROR(Nte0109, "the % s [[%s]] has only been used as a scope to find function [[%s]]");
    SWAG_ERROR(Nte0110, "the %s [[%s]] is waiting for %s [[%s]] to be solved");
    SWAG_ERROR(Nte0111, "the %s [[%s]] wasn't found in [[%s]]. The alternative from [[%s]] was selected");
    SWAG_ERROR(Nte0112, "the [[dref]] operation isn't possible here");
    SWAG_ERROR(Nte0113, "the array has %d element(s) of type [[%s]], which doesn't match a slice of type [[%s]]");
    SWAG_ERROR(Nte0114, "the call to [[%s]] returns type [[%s]], which doesn't have a sub-scope");
    SWAG_ERROR(Nte0115, "the counterpart is an implicit [[using self]] as an initial parameter");
    SWAG_ERROR(Nte0116, "the duplicated underlying enum value is [[%s]]");
    SWAG_ERROR(Nte0117, "the function [[%s]] is not marked with the [[#[Swag.ConstExpr]]] attribute");
    SWAG_ERROR(Nte0118, "the function [[%s]] is tagged with [[#[Swag.CalleeReturn]]], implying the return value is utilized within [[%s]]");
    SWAG_ERROR(Nte0119, "the function call returns an immutable [[%s]]");
    SWAG_ERROR(Nte0120, "the function is tagged with the [[#[Swag.Inline]]] attribute");
    SWAG_ERROR(Nte0121, "the function is tagged with the [[#[Swag.Macro]]] attribute");
    SWAG_ERROR(Nte0122, "the function is tagged with the [[#[Swag.Mixin]]] attribute");
    SWAG_ERROR(Nte0123, "the interface declaration returns type [[%s]]");
    SWAG_ERROR(Nte0124, "the interface declaration yields no return");
    SWAG_ERROR(Nte0125, "the keyword [[%s]] can't be used as an identifier");
    SWAG_ERROR(Nte0126, "the missing value can be found here");
    SWAG_ERROR(Nte0127, "the number of values ([[%d]]) is greater than one");
    SWAG_ERROR(Nte0128, "the number of values is variable and could be greater than one");
    SWAG_ERROR(Nte0129, "the operation is not allowed on a non-pointer types");
    SWAG_ERROR(Nte0130, nullptr);
    SWAG_ERROR(Nte0131, "the operator [[++]] requires compile-time strings as arguments");
    SWAG_ERROR(Nte0132, "the parent scope for [[impl]] is [[%s]], but the parent scope for [[%s]] is [[%s]]");
    SWAG_ERROR(Nte0133, "the resulting type is [[%s]]");
    SWAG_ERROR(Nte0134, "the return type of an [[#ast]] block should be of type [[string]]");
    SWAG_ERROR(Nte0135, "the slicing lower bound type is invalid, expected an integer, got [[%s]] instead");
    SWAG_ERROR(Nte0136, "the symbol [[%s]] is already present in the interface scope [[%s]]");
    SWAG_ERROR(Nte0137, "the symbol [[%s]] was located through a [[using]] statement");
    SWAG_ERROR(Nte0138, "the symbol [[...]] is used to declare variadic function parameters, which is not valid in this context");
    SWAG_ERROR(Nte0139, "the type [[%s]] doesn't meet the constraint [[%s]]");
    SWAG_ERROR(Nte0140, "the type [[%s]] fails to meet the given constraint");
    SWAG_ERROR(Nte0141, "the type [[%s]] is expected due to %s");
    SWAG_ERROR(Nte0142, "the version format should be [[version.revision.buildnum]], where each number is either >= 0 or the [[?]] character");
    SWAG_ERROR(Nte0143, "there's an hidden call to [[%s]] for the type [[%s]]");
    SWAG_ERROR(Nte0144, "there's an hidden call to [[%s]]");
    SWAG_ERROR(Nte0145, "there's an implied first parameter [[self]]");
    SWAG_ERROR(Nte0146, "this %s has [[%s]] access");
    SWAG_ERROR(Nte0147, "this %s has the [[#[Swag.Compiler]]] attribute, which makes it compile-time only");
    SWAG_ERROR(Nte0148, "this %s has type [[%s]]");
    SWAG_ERROR(Nte0149, "this [[discard]] should be removed");
    SWAG_ERROR(Nte0150, "this [[using]] field is convertible");
    SWAG_ERROR(Nte0151, "this argument has been named");
    SWAG_ERROR(Nte0152, "this can be converted too");
    SWAG_ERROR(Nte0153, "this function call can't be evaluated at compile-time");
    SWAG_ERROR(Nte0154, "this function does not support UFCS of type [[%s]]");
    SWAG_ERROR(Nte0155, "this function does not support aliased names");
    SWAG_ERROR(Nte0156, "this function has the [[#[Swag.Compiler]]] attribute, which makes it compile-time only");
    SWAG_ERROR(Nte0157, "this has [[%s]] access (type is [[%s]])");
    SWAG_ERROR(Nte0158, "this is a constant");
    SWAG_ERROR(Nte0159, "this is a lambda expression");
    SWAG_ERROR(Nte0160, "this is a pointer type declaration due to [[*]]");
    SWAG_ERROR(Nte0161, "this is a type value, aka [[typeinfo]], aka [[%s]]");
    SWAG_ERROR(Nte0162, "this is an unnamed [[?]] parameter");
    SWAG_ERROR(Nte0163, "this is the expected signature");
    SWAG_ERROR(Nte0164, "this is the first initialization");
    SWAG_ERROR(Nte0165, "this is the first usage");
    SWAG_ERROR(Nte0166, "this is the literal suffix [[%s]]");
    SWAG_ERROR(Nte0167, "this is unexpected in global scope");
    SWAG_ERROR(Nte0168, "this list of parameters are missing their type");
    SWAG_ERROR(Nte0169, "this parameter is missing its type");
    SWAG_ERROR(Nte0170, "this previous parameter has a default value");
    SWAG_ERROR(Nte0171, "this should be designated as [[const %s]]");
    SWAG_ERROR(Nte0172, "this should be lower than this");
    SWAG_ERROR(Nte0173, "this slice appears to be null or empty");
    SWAG_ERROR(Nte0174, "this string appears to be null or empty");
    SWAG_ERROR(Nte0175, "this tuple has [[%d]] field(s)");
    SWAG_ERROR(Nte0176, "this tuple seems to be empty");
    SWAG_ERROR(Nte0177, "this type is [[%s]]");
    SWAG_ERROR(Nte0178, "this value can only be converted to type [[%s]] with a dynamic call to [[opAffect]]");
    SWAG_ERROR(Nte0179, "to begin a new block, consider moving [[{]] to a new line");
    SWAG_ERROR(Nte0180, "to close this you need [[%s]]");
    SWAG_ERROR(Nte0181, "to declare a variable, the syntax is [[var name: type]] or [[var name = expression]]");
    SWAG_ERROR(Nte0182, "to dereference variable [[%s]] as in C, employ [[dref %s]]");
    SWAG_ERROR(Nte0183, "to initiate a struct initialization of [[%s]], remove spaces between [[%s]] and [[{]]");
    SWAG_ERROR(Nte0184, "to retrieve the program arguments, consider using the [[@args()]] intrinsic");
    SWAG_ERROR(Nte0185, "trying to match the type of the other part of the conditional expression");
    SWAG_ERROR(Nte0186, "unable to resolve %s [[%s]]");
    SWAG_ERROR(Nte0187, "unable to resolve type [[%s]]");
    SWAG_ERROR(Nte0188, "unnamed parameters ([[?]]) should not have an explicitly specified type");
    SWAG_ERROR(Nte0189, "use [[#type]] before [[%s]] if it's a type, or specify a type with [[:]] if this is a parameter name");
    SWAG_ERROR(Nte0190, "use [[-w:<path>]] or [[--workspace:<path>]] to specify a valid workspace folder or run [[Swag]] from an appropriate workspace");
    SWAG_ERROR(Nte0191, "you can assign a lambda to a closure type, but not vice versa");
    SWAG_ERROR(Nte0192, "you can execute [[Swag]] with [[--callstack]] to obtain more contextual details");
    SWAG_ERROR(Nte0193, "you can execute [[Swag]] with [[--dbg-catch]] to initiate the bytecode debugger when an exception is raised");
    SWAG_ERROR(Nte0194, "you can't reference this runtime %s from the compile-time %s");
    SWAG_ERROR(Nte0195, "you might want to get the address of [[%s]] using [[&]]");
    SWAG_ERROR(Nte0196, "you need to take the address of a value to make a reference");
    SWAG_ERROR(Nte0197, "the value could come from [[%s]]");
    SWAG_ERROR(Nte0198, "this suffix wants the literal to be converted to [[%s]]");
    SWAG_ERROR(Nte0199, "the identifier [[%s]] is %s and not a function or a struct");
    SWAG_ERROR(Nte0200, "expected an integer, a rune or a float value");
}

Utf8 Err(ErrorID idx)
{
    return FMT("[%s] %s", g_EI[idx], g_E[idx].c_str());
}

Utf8 ErrNte(ErrorID idx, bool forNote)
{
    if (forNote)
        return Nte(idx);
    return Err(idx);
}

Utf8 Nte(ErrorID idx)
{
    return g_E[idx];
}
