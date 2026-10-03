#pragma once
/**
 * @file ai_draft_dispatch.h
 * @brief API-key resolution + AI call for
 *        AiDraftController.
 */

#include "ai-chat/backend/AiSuggestionService.h"
#include "api-keys/backend/ApiKeyService.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <functional>
#include <string>

namespace controllers
{

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

/// Singleton suggestion service for drafts.
inline services::AiSuggestionService& draftSvc()
{
    static services::AiSuggestionService s;
    return s;
}

/**
 * @brief Resolve API key and call AI for a document.
 *
 * @param userId   Authenticated user ID.
 * @param docType  Document type slug.
 * @param type     Startup type.
 * @param stage    Startup stage.
 * @param provider AI provider enum.
 * @param prompt   Filled prompt string.
 * @param cb       Response callback.
 */
inline void dispatchDraft(
    const std::string& userId,
    const std::string& docType,
    const std::string& type,
    const std::string& stage,
    services::AiProvider provider,
    const std::string& prompt,
    Cb cb)
{
    services::ApiKeyService::resolve(
        userId, provider,
        [userId, docType, type, stage,
         provider, prompt, cb](json resolved) {
            const auto key =
                resolved.value("apiKey", "");
            const auto model =
                resolved.value("model", "");
            if (key.empty()) {
                cb(::utils::jsonError(
                    drogon::k503ServiceUnavailable,
                    "No API key", "DRAFT_003"));
                return;
            }
            draftSvc().suggest(
                userId, prompt, "",
                type, stage, provider,
                key, model,
                [docType, cb](json r) {
                    cb(::utils::jsonCreated({
                        {"document_type", docType},
                        {"content",
                         r.value("suggestion",
                                 "")},
                        {"generated_at", "now"},
                    }));
                },
                [cb](drogon::HttpStatusCode s,
                     std::string m) {
                    cb(::utils::jsonError(
                        s, m, "DRAFT_004"));
                });
        },
        [cb](drogon::HttpStatusCode s,
             std::string m) {
            cb(::utils::jsonError(
                s, m, "DRAFT_003"));
        });
}

} // namespace controllers
