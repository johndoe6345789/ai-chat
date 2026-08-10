#pragma once
/**
 * @file ai_openai_parser.h
 * @brief Response parser for OpenAI ChatCompletion.
 */

#include "ai-chat/backend/AiTypes.h"

#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

namespace services::detail
{

/**
 * @brief Parse an OpenAI ChatCompletion response.
 * @param resp      HTTP response from OpenAI.
 * @param dfltModel Default model name fallback.
 * @param onOk      Success callback.
 * @param onErr     Error callback.
 */
inline void parseOpenAiResponse(
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
                           "Unknown OpenAI error");
            spdlog::error(
                "OpenAI API error: {}", msg);
            onErr(drogon::k502BadGateway, msg);
            return;
        }
        std::string content;
        if (j.contains("choices")
            && j["choices"].is_array()
            && !j["choices"].empty()) {
            content =
                j["choices"][0]
                    .value("message",
                           json::object())
                    .value("content", "");
        }
        std::int64_t tokens = 0;
        if (j.contains("usage")) {
            tokens = j["usage"].value(
                "total_tokens",
                static_cast<std::int64_t>(0));
        }
        onOk({{"content", content},
              {"model",
               j.value("model", dfltModel)},
              {"tokensUsed", tokens}});
    } catch (const std::exception& e) {
        spdlog::error(
            "OpenAI parse error: {}", e.what());
        onErr(drogon::k502BadGateway,
              "Invalid OpenAI API response");
    }
}

} // namespace services::detail
