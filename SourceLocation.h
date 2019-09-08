#pragma once
static const int REPORT_NUM_CODE_LINES = 3;

struct SourceLocation
{
    int line;
    int column;
    int seekStartLine[REPORT_NUM_CODE_LINES];
};
