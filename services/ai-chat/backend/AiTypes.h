#pragma once
/**
 * @file AiTypes.h
 * @brief Shared types and aliases for the AI chat subsystem.
 *
 * Defines the AiProvider enum, AiResponse value object, and
 * common callback type aliases used across the AI service
 * modules.
 */

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <functional>
#include <string>

namespace services
{

using json = nlohmann::json;
using DbClientPtr = drogon::orm::DbClientPtr;
using Callback = std::function<void(json)>;
using ErrCallback = std::function<void(drogon::HttpStatusCode, std::string)>;

/**
 * @brief Supported AI provider backends.
 */
enum class AiProvider : std::uint8_t {
    CLAUDE = 0, ///< Anthropic Claude API.
    OPENAI = 1  ///< OpenAI ChatCompletion API.
};

/**
 * @brief Convert a string to AiProvider enum.
 *
 * @param s  "claude" or "openai" (case-insensitive).
 * @return Matching enum value; defaults to CLAUDE.
 */
[[nodiscard]] inline auto parseProvider(const std::string& s) -> AiProvider
{
    if (s == "openai" || s == "OPENAI") {
        return AiProvider::OPENAI;
    }
    return AiProvider::CLAUDE;
}

/**
 * @brief Response value object from an AI chat request.
 */
struct AiResponse {
    std::string role{"assistant"};
    std::string content;
    std::string provider;
    std::string model;
    std::int64_t tokensUsed{0};
};

} // namespace services
