/**
 * @file AiRiskReportController.cpp
 * @brief Implementation of the AI risk-report endpoint.
 */

#include "AiRiskReportController.h"
#include "ai_risk_helpers.h"
#include "ai-chat/backend/AiSuggestionService.h"
#include "api-keys/backend/ApiKeyService.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

static services::AiSuggestionService riskSvc;

void AiRiskReportController::report(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    const auto userId =
        req->attributes()->get<std::string>("user_id");

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);
    if (body.is_discarded()) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "Invalid JSON body"));
        return;
    }

    const auto provider = services::parseProvider(
        body.value("provider", "claude"));
    const auto type =
        body.value("startup_type", "startup");
    const auto stage =
        body.value("stage", "early");

    services::ApiKeyService::resolve(
        userId, provider,
        [userId, body, type, stage,
         provider, cb](json resolved) {
            const auto apiKey =
                resolved.value("apiKey", "");
            const auto model =
                resolved.value("model", "");
            if (apiKey.empty()) {
                cb(::utils::jsonError(
                    drogon::k503ServiceUnavailable,
                    "No API key configured",
                    "RISK_001"));
                return;
            }
            const std::string prompt =
                buildRiskPrompt(body);
            riskSvc.suggest(
                userId, prompt, "",
                type, stage, provider,
                apiKey, model,
                [cb](json r) {
                    cb(::utils::jsonCreated({
                        {"risks",
                         r.value("suggestion",
                                 "")},
                        {"generated_at", "now"},
                    }));
                },
                [cb](drogon::HttpStatusCode s,
                     std::string m) {
                    cb(::utils::jsonError(
                        s, m, "RISK_002"));
                });
        },
        [cb](drogon::HttpStatusCode s,
             std::string m) {
            cb(::utils::jsonError(
                s, m, "RISK_001"));
        });
}

} // namespace controllers
