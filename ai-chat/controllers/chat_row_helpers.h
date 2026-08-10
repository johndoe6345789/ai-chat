#pragma once
/**
 * @file chat_row_helpers.h
 * @brief Helper to build chat message JSON from a
 *        DB row.
 */

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>

namespace controllers::detail
{

/**
 * @brief Build a chat message JSON from a row.
 * @param row Database result row.
 * @return JSON object with message fields.
 */
inline auto chatMsgFromRow(
    const drogon::orm::Row& row)
    -> nlohmann::json
{
    return {
        {"id",
         row["id"].as<std::string>()},
        {"role",
         row["role"].as<std::string>()},
        {"content",
         row["content"].as<std::string>()},
        {"provider",
         row["provider"].as<std::string>()},
        {"timestamp",
         row["created_at"]
             .as<std::string>()},
    };
}

} // namespace controllers::detail
