#pragma once
/**
 * @file chat_send_handler.h
 * @brief Request parsing helpers for ChatController.
 */

#include "drogon-host/backend/utils/JsonResponse.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace controllers
{

using json = nlohmann::json;

/**
 * @brief Parsed chat send request fields.
 */
struct ChatSendInput {
    std::string userId;
    std::string message;
    std::string providerStr;
};

/**
 * @brief Parse and validate a chat send request.
 * @param req The HTTP request.
 * @return Parsed input or nullopt if invalid.
 */
inline auto parseChatRequest(
    const drogon::HttpRequestPtr& req)
    -> std::optional<ChatSendInput>
{
    ChatSendInput input;
    input.userId = req->attributes()
        ->get<std::string>("user_id");

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);
    if (body.is_discarded()
        || !body.contains("message")) {
        return std::nullopt;
    }

    input.message =
        body["message"].get<std::string>();
    input.providerStr =
        body.value("provider", "claude");

    if (input.message.empty()) {
        return std::nullopt;
    }
    return input;
}

} // namespace controllers
