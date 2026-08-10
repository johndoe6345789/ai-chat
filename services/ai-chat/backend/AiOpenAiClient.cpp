/**
 * @file AiOpenAiClient.cpp
 * @brief OpenAI ChatCompletion HTTP client.
 */

#include "ai-chat/backend/AiOpenAiClient.h"
#include "ai_openai_parser.h"

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <cstdlib>

namespace services
{

using namespace drogon;
AiOpenAiClient::AiOpenAiClient()
{
    const char* v =
        std::getenv("OPENAI_API_KEY");
    apiKey_ = v ? v : "";
    if (apiKey_.empty())
        spdlog::warn("OPENAI_API_KEY not set");
}

void AiOpenAiClient::call(
    const json& messages,
    Callback onSuccess,
    ErrCallback onError)
{
    call(messages, apiKey_, "",
         onSuccess, onError);
}

void AiOpenAiClient::call(
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
                "OpenAI API key not configured");
        return;
    }
    const auto mdl =
        model.empty() ? kModel : model.c_str();

    auto client = HttpClient::newHttpClient(
        "https://api.openai.com");
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath("/v1/chat/completions");
    req->addHeader("Authorization",
                   fmt::format("Bearer {}", key));
    req->setContentTypeCode(CT_APPLICATION_JSON);

    json body = {{"model", mdl},
                 {"messages", messages},
                 {"max_tokens", kMaxTokens}};
    req->setBody(body.dump());

    client->sendRequest(
        req,
        [onSuccess, onError](
            ReqResult result,
            const HttpResponsePtr& resp) {
            if (result != ReqResult::Ok
                || !resp) {
                spdlog::error(
                    "OpenAI HTTP request failed");
                onError(k502BadGateway,
                        "Failed to reach "
                        "OpenAI API");
                return;
            }
            detail::parseOpenAiResponse(
                resp, kModel,
                onSuccess, onError);
        });
}

} // namespace services
