#include "DebugLogger.h"
#include <fstream>

// Define global instance
DebugLogger g_DebugLogger("debug_log.json");

DebugLogger::DebugLogger(const std::string& filename)
    : m_filename(filename) {
    m_debugData["debug"] = nlohmann::json::object();
}

void DebugLogger::LogFrame(int fps, const std::string& lastKeyPress) {
    m_debugData["debug"]["fps"] = fps;
    m_debugData["debug"]["lastKeyPress"] = lastKeyPress;
}

void DebugLogger::Update(float deltaTime) {
    m_saveTimer += deltaTime;
    if (m_saveTimer >= 5.0f) {
        std::ofstream file(m_filename);
        if (file.is_open()) {
            file << m_debugData.dump(4);
        }
        m_saveTimer = 0.0f;
    }
}
