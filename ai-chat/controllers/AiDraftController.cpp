/**
 * @file AiDraftController.cpp
 * @brief AI draft-document endpoint implementation.
 */

#include "AiDraftController.h"
#include "ai_draft_helpers.h"
#include "ai_draft_dispatch.h"

#include "ai_json.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void AiDraftController::draft(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    const auto userId =
        req->attributes()->get<std::string>("user_id");
    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);
    if (body.is_discarded() || !body.is_object()
        || strOr(body, "document_type", "").empty()) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "document_type required",
            "DRAFT_001"));
        return;
    }
    const auto docType = strOr(body, "document_type", "");
    auto it = kDraftTemplates.find(docType);
    if (it == kDraftTemplates.end()) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "Unknown document_type",
            "DRAFT_002"));
        return;
    }
    const auto name =
        strOr(body, "startup_name", "your startup");
    const auto type =
        strOr(body, "startup_type", "startup");
    const auto stage =
        strOr(body, "stage", "early");
    const auto provider = services::parseProvider(
        strOr(body, "provider", defaultProvider()));
    dispatchDraft(
        userId, docType, type, stage,
        provider,
        fillDraftTemplate(
            it->second, name, type, stage),
        std::move(cb));
}

} // namespace controllers
