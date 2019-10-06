#pragma once
enum class BackendType
{
    Exe,
    Dll,
    Lib,
};

struct BackendParameters
{
    BackendType    type              = BackendType::Exe;
    bool           debugInformations = false;
    bool           optimize          = false;
    string         postFix;
    vector<string> defines;
};
