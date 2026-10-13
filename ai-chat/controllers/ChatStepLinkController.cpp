/**
 * @file ChatStepLinkController.cpp
 * @brief Chat-step-link endpoint implementation.
 */

#include "ChatStepLinkController.h"
#include "chat_step_link_handlers.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

static auto db() -> drogon::orm::DbClientPtr
{
    return drogon::app().getDbClient();
}

void ChatStepLinkController::getLink(
    const drogon::HttpRequestPtr& req,
    Cb&& cb,
    std::string stepId)
{
    const auto userId =
        req->attributes()->get<std::string>("user_id");
    queryStepLink(db(), userId, stepId,
                  std::move(cb));
}

void ChatStepLinkController::saveLink(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    const auto userId =
        req->attributes()->get<std::string>("user_id");

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);
    if (body.is_discarded()
        || !body.contains("step_id")
        || !body.contains("context")) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "step_id and context required"));
        return;
    }
    const auto stepId =
        body["step_id"].get<std::string>();
    const auto context =
        body["context"].get<std::string>();
    spdlog::debug(
        "saveLink user={} step={}",
        userId, stepId);
    insertStepLink(db(), userId, stepId,
                   context, std::move(cb));
}

} // namespace controllers
