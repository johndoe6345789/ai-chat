/**
 * @file ChatHistoryClear.cpp
 * @brief Endpoint to clear all chat history.
 */

#include "ChatController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void ChatController::clearHistory(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    auto userId = req->attributes()
        ->get<std::string>("user_id");
    auto dbClient =
        drogon::app().getDbClient();

    const std::string sql = R"(
        DELETE FROM chat_messages
        WHERE user_id = $1
    )";

    *dbClient << sql << userId
        >> [cb](const drogon::orm::Result&) {
            cb(::utils::jsonOk(
                {{"message",
                  "Chat history cleared"}}));
        }
        >> [cb](const drogon::orm::
                    DrogonDbException& e) {
            spdlog::error(
                "clearHistory: {}",
                e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "Failed to clear history"));
        };
}

} // namespace controllers
