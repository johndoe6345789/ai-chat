/**
 * @file ChatHistoryList.cpp
 * @brief Paginated chat history retrieval.
 */

#include "ChatController.h"
#include "chat_row_helpers.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "drogon-host/backend/utils/parse_helpers.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void ChatController::history(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    auto userId = req->attributes()
        ->get<std::string>("user_id");
    int64_t page = ::utils::safeStoll(
        req->getParameter("page"), 1);
    int64_t perPage = ::utils::safeStoll(
        req->getParameter("per_page"), 50);
    int64_t offset = (page - 1) * perPage;
    auto db = drogon::app().getDbClient();

    const std::string cSql = R"(
        SELECT COUNT(*) AS total
        FROM chat_messages WHERE user_id = $1
    )";

    *db << cSql << userId
        >> [db, userId, page, perPage,
            offset, cb](
               const drogon::orm::Result& cr) {
            int64_t total =
                cr[0]["total"].as<int64_t>();
            const std::string sql = R"(
                SELECT id, role, content,
                       provider, created_at
                FROM chat_messages
                WHERE user_id = $1
                ORDER BY created_at ASC
                LIMIT $2 OFFSET $3
            )";
            *db << sql << userId
                << perPage << offset
                >> [total, page, perPage, cb](
                       const drogon::orm::Result& r) {
                    json msgs = json::array();
                    for (const auto& row : r)
                        msgs.push_back(
                            detail::chatMsgFromRow(
                                row));
                    cb(::utils::jsonPaginated(
                        msgs, total, page,
                        perPage));
                }
                >> [cb](const drogon::orm::
                            DrogonDbException& e) {
                    spdlog::error(
                        "chat history: {}",
                        e.base().what());
                    cb(::utils::jsonError(
                        drogon::k500InternalServerError,
                        "Failed to load history"));
                };
        }
        >> [cb](const drogon::orm::
                    DrogonDbException& e) {
            spdlog::error("chat count: {}",
                          e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "Failed to load history"));
        };
}

} // namespace controllers
