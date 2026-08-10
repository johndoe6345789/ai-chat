#pragma once
/**
 * @file ai_claude_parser.h
 * @brief Response parser for Anthropic Claude API.
 */

#include "ai-chat/backend/AiTypes.h"

#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

namespace services::detail
{

/**
 * @brief Parse an Anthropic Claude response.
 * @param resp      HTTP response from Claude.
 * @param dfltModel Default model name fallback.
 * @param onOk      Success callback.
 * @param onErr     Error callback.
 */
inline void parseClaudeResponse(
    const drogon::HttpResponsePtr& resp,
    const char* dfltModel,
    Callback onOk,
    ErrCallback onErr)
{
    try {
        auto j = json::parse(resp->getBody());
        if (resp->statusCode()
            != drogon::k200OK) {
            auto msg =
                j.value("error", json::object())
                    .value("message",
                           "Unknown Claude error");
            spdlog::error(
                "Claude API error: {}", msg);
            onErr(drogon::k502BadGateway, msg);
            return;
        }
        std::string content;
        if (j.contains("content")
            && j["content"].is_array()
            && !j["content"].empty()) {
            content = j["content"][0]
                          .value("text", "");
        }
        std::int64_t tokens = 0;
        if (j.contains("usage")) {
            tokens =
                j["usage"].value(
                    "input_tokens",
                    static_cast<std::int64_t>(0))
                + j["usage"].value(
                    "output_tokens",
                    static_cast<std::int64_t>(0));
        }
        onOk({{"content", content},
              {"model",
               j.value("model", dfltModel)},
              {"tokensUsed", tokens}});
    } catch (const std::exception& e) {
        spdlog::error(
            "Claude parse error: {}", e.what());
        onErr(drogon::k502BadGateway,
              "Invalid Claude API response");
    }
}

} // namespace services::detail
