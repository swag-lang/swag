#pragma once
class Module
{
public:
    Module(const fs::path& path)
        : m_path{path}
    {
		m_name = path.filename().string();
    }

private:
    fs::path m_path;
    string   m_name;
};
