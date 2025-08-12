#pragma once
#include <string>
#include <json.hpp>

class DebugLogger {
public:
    DebugLogger(const std::string& filename);
    void LogFrame(int fps, const std::string& lastKeyPress);
    void Update(float deltaTime);

private:
    std::string m_filename;
    nlohmann::json m_debugData;
    float m_saveTimer = 0.0f;
};

// Declare global instance (extern)
extern DebugLogger g_DebugLogger;
