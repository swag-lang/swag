#include "pch.h"
#include "Log.h"
#include "ByteCodeDebugger.h"

void ByteCodeDebugger::printHelp()
{
    g_Log.setColor(LogColor::Gray);
    g_Log.eol();

    g_Log.print("<return>                      'step', runs to the next line or instruction (depends on 'bcmode')\n");
    g_Log.print("<shift+return>                'next', runs to the next line or instruction (depends on 'bcmode')\n");
    g_Log.print("<tab>                         contextual completion of the current word\n");
    g_Log.eol();

    g_Log.print("s(tep)                        runs to the next line\n");
    g_Log.print("n(ext)                        like s, but does not step into functions or inlined code\n");
    g_Log.print("f(inish)                      runs until the current function is done\n");
    g_Log.print("c(ont(inue))                  runs until another breakpoint is reached\n");
    g_Log.print("un(til) <num>                 runs to the given line or instruction in the current function (depends on 'bcmode')\n");
    g_Log.print("j(ump)  <num>                 jump to the given line or instruction in the current function (depends on 'bcmode')\n");
    g_Log.eol();

    g_Log.print("l(ist) [num]                  print the current source code line and [num] lines around\n");
    g_Log.print("ll [name]                     print the current function (or function [name]) source code\n");
    g_Log.eol();

    g_Log.print("e(xec(ute)) <stmt>            execute the Swag code statement <stmt> in the current context\n");
    g_Log.print("$<expr|stmt>                  execute the Swag code expression <expr> or statement <stmt> in the current context\n");
    g_Log.print("<expr|stmt>                   execute the Swag code expression <expr> or statement <stmt> in the current context (if not a valid debugger command)\n");
    g_Log.eol();

    g_Log.print("p(rint) [@format] <expr>      print the value of the Swag code expression <expr> in the current context (format is the same as 'x' command)\n");
    g_Log.print("info locals                   print all current local variables\n");
    g_Log.print("info args                     print all current function arguments\n");
    g_Log.print("info func [filter]            print all functions which contains [filter] in their names\n");
    g_Log.print("info modules                  print all modules\n");
    g_Log.print("info regs [@format]           print all registers (format is the same as 'x' command)\n");
    g_Log.print("(w)here                       print contextual informations\n");
    g_Log.eol();

    g_Log.print("x [@format] [@num] <expr>     print memory (format = s8|s16|s32|s64|u8|u16|u32|u64|x8|x16|x32|x64|f32|f64)\n");
    g_Log.eol();

    g_Log.print("b(reak)                       print all breakpoints\n");
    g_Log.print("b(reak) func <name>           add breakpoint when entering function <name> in the current file\n");
    g_Log.print("b(reak) <line>                add breakpoint in the current source file at <line>\n");
    g_Log.print("b(reak) <file>:<line>         add breakpoint in <file> at <line>\n");
    g_Log.print("b(reak) clear                 remove all breakpoints\n");
    g_Log.print("b(reak) clear <num>           remove breakpoint <num>\n");
    g_Log.print("b(reak) enable <num>          enable breakpoint <num>\n");
    g_Log.print("b(reak) disable <num>         disable breakpoint <num>\n");
    g_Log.print("tb(reak) ...                  same as 'b(reak)' except that the breakpoint will be automatically removed on hit\n");
    g_Log.eol();

    g_Log.print("bt                            backtrace, print callstack\n");
    g_Log.print("u(p)   [num]                  move stack frame <num> level up\n");
    g_Log.print("d(own) [num]                  move stack frame <num> level down\n");
    g_Log.print("frame  <num>                  move stack frame to level <num>\n");
    g_Log.eol();

    g_Log.print("bcmode                        swap between bytecode mode and source code mode\n");
    g_Log.print("i [num]                       print the current bytecode instruction and [num] instructions around\n");
    g_Log.print("ii                            print the current function bytecode\n");
    g_Log.eol();

    g_Log.print("?                             print this list of commands\n");
    g_Log.print("q(uit)                        quit the compiler\n");
    g_Log.eol();
}
