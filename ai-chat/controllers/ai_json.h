#pragma once
/**
 * @file ai_json.h
 * @brief Null-tolerant reads of request-body fields.
 *
 * nlohmann's `value(key, default)` throws when the key is present but
 * null, and the frontend sends null for things the person has not chosen
 * yet (e.g. startup_type). That used to surface as a 500 on every AI call.
 */

#include <nlohmann/json.hpp>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <string>

namespace controllers
{

/** @brief String field, or @p dflt when missing, null or not a string. */
[[nodiscard]] inline std::string strOr(
    const nlohmann::json& j, const char* key, std::string dflt)
{
    if (!j.is_object()) return dflt;
    const auto it = j.find(key);
    if (it == j.end() || !it->is_string()) return dflt;
    return it->get<std::string>();
}

/** @brief Integer field, or @p dflt when missing, null or not a number. */
[[nodiscard]] inline std::int64_t intOr(
    const nlohmann::json& j, const char* key, std::int64_t dflt)
{
    if (!j.is_object()) return dflt;
    const auto it = j.find(key);
    if (it == j.end() || !it->is_number()) return dflt;
    return it->get<std::int64_t>();
}

/** @brief The current time as ISO 8601 UTC (what the frontend parses). */
[[nodiscard]] inline std::string isoNow()
{
    return std::format("{:%Y-%m-%dT%H:%M:%SZ}",
        std::chrono::floor<std::chrono::seconds>(
            std::chrono::system_clock::now()));
}

/**
 * @brief Provider used when a request names none: AI_DEFAULT_PROVIDER
 *        ("openai" or "claude"), else "claude". The frontend sends none, so
 *        this decides which configured key/server serves every AI call.
 */
[[nodiscard]] inline std::string defaultProvider()
{
    const char* v = std::getenv("AI_DEFAULT_PROVIDER");
    return v && *v ? v : "claude";
}

}  // namespace controllers
