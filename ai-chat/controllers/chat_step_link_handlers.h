#pragma once
/**
 * @file chat_step_link_handlers.h
 * @brief DB helpers for ChatStepLinkController.
 */

#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>
#include <functional>
#include <string>

namespace controllers
{

using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

/**
 * @brief Query the most recent context for a step.
 * @param db userId stepId cb  Standard args.
 */
inline void queryStepLink(
    const drogon::orm::DbClientPtr& db,
    const std::string& userId,
    const std::string& stepId,
    Cb cb)
{
    db->execSqlAsync(
        "SELECT context, created_at "
        "FROM chat_step_links "
        "WHERE user_id=$1 AND step_id=$2 "
        "ORDER BY created_at DESC LIMIT 1",
        [cb](const drogon::orm::Result& r) {
            if (r.empty()) {
                cb(::utils::jsonOk(
                    {{"context", ""}}));
                return;
            }
            cb(::utils::jsonOk({
                {"context",
                 r[0]["context"]
                     .as<std::string>()},
                {"created_at",
                 r[0]["created_at"]
                     .as<std::string>()},
            }));
        },
        [cb](
            const drogon::orm::DrogonDbException&
                e) {
            spdlog::error("getLink DB: {}",
                          e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "Database error"));
        },
        userId, stepId);
}

/**
 * @brief Insert a new step-link row.
 * @param db userId stepId context cb  Standard args.
 */
inline void insertStepLink(
    const drogon::orm::DbClientPtr& db,
    const std::string& userId,
    const std::string& stepId,
    const std::string& context,
    Cb cb)
{
    db->execSqlAsync(
        "INSERT INTO chat_step_links"
        " (user_id, step_id, context)"
        " VALUES ($1,$2,$3)",
        [cb](const drogon::orm::Result&) {
            cb(::utils::jsonCreated(
                {{"ok", true}}));
        },
        [cb](
            const drogon::orm::DrogonDbException&
                e) {
            spdlog::error("saveLink DB: {}",
                          e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "Database error"));
        },
        userId, stepId, context);
}

} // namespace controllers
