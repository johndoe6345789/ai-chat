/**
 * @file ChatController.cpp
 * @brief AI chat endpoint implementations.
 */

#include "ChatController.h"
#include "ai-chat/backend/AiService.h"
#include "api-keys/backend/ApiKeyService.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "chat_send_handler.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

static services::AiService aiService;

void ChatController::sendMessage(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    auto input = parseChatRequest(req);
    if (!input.has_value()) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "message field required",
            "VAL_004"));
        return;
    }

    auto provider = services::parseProvider(
        input->providerStr);

    services::ApiKeyService::resolve(
        input->userId, provider,
        [userId = input->userId,
         message = input->message,
         provider, cb](json resolved) {
            auto apiKey =
                resolved.value("apiKey", "");
            auto model =
                resolved.value("model", "");

            if (apiKey.empty()) {
                cb(::utils::jsonError(
                    drogon::k503ServiceUnavailable,
                    "No API key configured",
                    "CHAT_001"));
                return;
            }

            aiService.chat(
                userId, message, provider,
                apiKey, model,
                [cb](json result) {
                    cb(::utils::jsonCreated(
                        result));
                },
                [cb](drogon::HttpStatusCode s,
                     std::string msg) {
                    cb(::utils::jsonError(
                        s, msg, "CHAT_002"));
                });
        },
        [cb](drogon::HttpStatusCode s,
             std::string msg) {
            cb(::utils::jsonError(
                s, msg, "CHAT_001"));
        });
}

} // namespace controllers
