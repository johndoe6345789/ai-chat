/**
 * @file AiSuggestionController.cpp
 * @brief Implementation of the step-suggestion AI endpoint.
 */

#include "AiSuggestionController.h"
#include "ai-chat/backend/AiSuggestionService.h"
#include "api-keys/backend/ApiKeyService.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include "ai_json.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(const drogon::HttpResponsePtr&)>;

namespace controllers
{

static services::AiSuggestionService suggestionSvc;

void AiSuggestionController::suggest(
    const drogon::HttpRequestPtr& req, Cb&& cb)
{
    const auto userId =
        req->attributes()->get<std::string>("user_id");

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);

    if (body.is_discarded() || !body.is_object()
        || strOr(body, "step_title", "").empty()) {
        cb(::utils::jsonError(drogon::k400BadRequest,
            "step_title field required", "SUGG_001"));
        return;
    }

    const auto stepTitle = strOr(body, "step_title", "");
    const auto stepDesc  = strOr(body, "step_description", "");
    const auto stype     = strOr(body, "startup_type", "startup");
    const auto stage     = strOr(body, "stage", "early");
    const auto provider  =
        services::parseProvider(strOr(body, "provider", defaultProvider()));

    spdlog::debug("AiSuggestionController user={}", userId);

    services::ApiKeyService::resolve(
        userId, provider,
        [userId, stepTitle, stepDesc, stype,
         stage, provider, cb](json resolved) {
            const auto apiKey = resolved.value("apiKey", "");
            const auto model  = resolved.value("model",  "");
            if (apiKey.empty()) {
                spdlog::warn("No API key user={}", userId);
                cb(::utils::jsonError(
                    drogon::k503ServiceUnavailable,
                    "No API key configured", "SUGG_002"));
                return;
            }
            suggestionSvc.suggest(
                userId, stepTitle, stepDesc,
                stype, stage, provider, apiKey, model,
                [cb](json r) {
                    cb(::utils::jsonCreated(r));
                },
                [cb](drogon::HttpStatusCode s,
                     std::string m) {
                    cb(::utils::jsonError(s, m, "SUGG_003"));
                });
        },
        [cb](drogon::HttpStatusCode s, std::string m) {
            cb(::utils::jsonError(s, m, "SUGG_002"));
        });
}

} // namespace controllers
