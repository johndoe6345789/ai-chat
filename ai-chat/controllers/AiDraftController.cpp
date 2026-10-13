/**
 * @file AiDraftController.cpp
 * @brief AI draft-document endpoint implementation.
 */

#include "AiDraftController.h"
#include "ai_draft_helpers.h"
#include "ai_draft_dispatch.h"

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
    if (body.is_discarded()
        || !body.contains("document_type")) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "document_type required",
            "DRAFT_001"));
        return;
    }
    const auto docType =
        body["document_type"].get<std::string>();
    auto it = kDraftTemplates.find(docType);
    if (it == kDraftTemplates.end()) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "Unknown document_type",
            "DRAFT_002"));
        return;
    }
    const auto name =
        body.value("startup_name", "your startup");
    const auto type =
        body.value("startup_type", "startup");
    const auto stage =
        body.value("stage", "early");
    const auto provider = services::parseProvider(
        body.value("provider", "claude"));
    dispatchDraft(
        userId, docType, type, stage,
        provider,
        fillDraftTemplate(
            it->second, name, type, stage),
        std::move(cb));
}

} // namespace controllers
