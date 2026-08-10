#pragma once
/**
 * @file ai_claude_request.h
 * @brief HTTP request builder for the Anthropic
 *        Claude Messages API.
 */

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <string>

namespace services
{

using json = nlohmann::json;

/**
 * @brief Build an HTTP request for Claude Messages.
 * @param key       API key for x-api-key header.
 * @param mdl       Model identifier.
 * @param messages  JSON array of messages.
 * @param maxTokens Max response tokens.
 * @return Configured HTTP request.
 */
inline auto buildClaudeRequest(
    const std::string& key,
    const char* mdl,
    const json& messages,
    std::int32_t maxTokens)
    -> drogon::HttpRequestPtr
{
    auto req =
        drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Post);
    req->setPath("/v1/messages");
    req->addHeader("x-api-key", key);
    req->addHeader(
        "anthropic-version", "2023-06-01");
    req->setContentTypeCode(
        drogon::CT_APPLICATION_JSON);

    json body = {{"model", mdl},
                 {"max_tokens", maxTokens},
                 {"messages", messages}};
    req->setBody(body.dump());
    return req;
}

} // namespace services
