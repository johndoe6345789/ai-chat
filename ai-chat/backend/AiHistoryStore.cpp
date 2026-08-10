/**
 * @file AiHistoryStore.cpp
 * @brief Implementation of AI chat message persistence.
 */

#include "ai-chat/backend/AiHistoryStore.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon::orm;

auto AiHistoryStore::db() -> DbClientPtr
{
    return drogon::app().getDbClient();
}

void AiHistoryStore::storeMessage(const std::string& userId,
                                  const std::string& role,
                                  const std::string& content,
                                  const std::string& provider,
                                  const std::string& model)
{
    auto dbClient = db();
    const std::string sql = R"(
        INSERT INTO chat_messages
            (user_id, role, content, provider,
             model, created_at)
        VALUES ($1, $2, $3, $4, $5, NOW())
    )";

    *dbClient << sql << userId << role << content << provider << model >>
        [](const Result&) {
            // Fire-and-forget.
        } >>
        [](const DrogonDbException& e) {
            spdlog::error("storeMessage DB error: {}", e.base().what());
        };
}

void AiHistoryStore::loadHistory(const std::string& userId, std::size_t limit,
                                 std::function<void(json)> callback)
{
    auto dbClient = db();
    const std::string sql = R"(
        SELECT role, content
        FROM chat_messages
        WHERE user_id = $1
        ORDER BY created_at DESC
        LIMIT $2
    )";

    *dbClient << sql << userId << static_cast<std::int64_t>(limit) >>
        [callback](const Result& result) {
            json history = json::array();
            // Results come newest-first; reverse them.
            for (auto it = result.rbegin(); it != result.rend(); ++it) {
                history.push_back(
                    {{"role", (*it)["role"].as<std::string>()},
                     {"content", (*it)["content"].as<std::string>()}});
            }
            callback(history);
        } >>
        [callback](const DrogonDbException& e) {
            spdlog::error("loadHistory DB error: {}", e.base().what());
            callback(json::array());
        };
}

} // namespace services
