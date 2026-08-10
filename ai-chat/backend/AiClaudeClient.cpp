/**
 * @file AiClaudeClient.cpp
 * @brief Anthropic Claude HTTP client.
 */

#include "ai-chat/backend/AiClaudeClient.h"
#include "ai_claude_parser.h"
#include "ai-chat/backend/ai_claude_request.h"

#include <drogon/HttpClient.h>
#include <spdlog/spdlog.h>

#include <cstdlib>

namespace services
{

using namespace drogon;

AiClaudeClient::AiClaudeClient()
{
    const char* v =
        std::getenv("ANTHROPIC_API_KEY");
    apiKey_ = v ? v : "";
    if (apiKey_.empty()) {
        spdlog::warn(
            "ANTHROPIC_API_KEY not set");
    }
}

void AiClaudeClient::call(
    const json& messages,
    Callback onSuccess,
    ErrCallback onError)
{
    call(messages, apiKey_, "",
         onSuccess, onError);
}

void AiClaudeClient::call(
    const json& messages,
    const std::string& apiKey,
    const std::string& model,
    Callback onSuccess,
    ErrCallback onError)
{
    const auto& key =
        apiKey.empty() ? apiKey_ : apiKey;
    if (key.empty()) {
        onError(k503ServiceUnavailable,
                "Claude API key not configured");
        return;
    }
    const auto mdl =
        model.empty() ? kModel : model.c_str();

    auto client = HttpClient::newHttpClient(
        "https://api.anthropic.com");
    auto req = buildClaudeRequest(
        key, mdl, messages, kMaxTokens);

    client->sendRequest(
        req,
        [onSuccess, onError](
            ReqResult result,
            const HttpResponsePtr& resp) {
            if (result != ReqResult::Ok
                || !resp) {
                spdlog::error(
                    "Claude HTTP request failed");
                onError(k502BadGateway,
                        "Failed to reach "
                        "Claude API");
                return;
            }
            detail::parseClaudeResponse(
                resp, kModel,
                onSuccess, onError);
        });
}

} // namespace services
