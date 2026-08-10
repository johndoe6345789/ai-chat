/**
 * @file AiService.cpp
 * @brief Thin facade orchestrating AI chat sub-modules.
 *
 * Delegates history persistence to AiHistoryStore and
 * provider calls to AiClaudeClient / AiOpenAiClient.
 */

#include "ai-chat/backend/AiService.h"

namespace services
{

// ----------------------------------------------------------------
// Construction
// ----------------------------------------------------------------

AiService::AiService() : historyStore_{}, claudeClient_{}, openAiClient_{} {}

// ----------------------------------------------------------------
// Public chat entry point
// ----------------------------------------------------------------

void AiService::chat(
    const std::string& userId,
    const std::string& message,
    AiProvider provider,
    const std::string& apiKey,
    const std::string& model,
    Callback onSuccess,
    ErrCallback onError)
{
    if (message.empty()) {
        onError(drogon::k400BadRequest,
                "Message must not be empty");
        return;
    }

    const std::string provStr =
        (provider == AiProvider::OPENAI)
            ? "openai" : "claude";

    historyStore_.storeMessage(
        userId, "user", message, provStr, "");

    historyStore_.loadHistory(
        userId, kHistoryLimit,
        [this, userId, message, provider,
         provStr, apiKey, model, onSuccess,
         onError](json history) {
            history.push_back(
                {{"role", "user"},
                 {"content", message}});

            auto onReply =
                [this, userId, provStr,
                 onSuccess](json reply) {
                    auto c = reply.value(
                        "content", "");
                    auto m = reply.value(
                        "model", "");
                    auto t = reply.value(
                        "tokensUsed",
                        static_cast<
                            std::int64_t>(0));
                    historyStore_.storeMessage(
                        userId, "assistant",
                        c, provStr, m);
                    onSuccess(
                        {{"role", "assistant"},
                         {"content", c},
                         {"provider", provStr},
                         {"model", m},
                         {"tokensUsed", t}});
                };

            if (provider == AiProvider::OPENAI) {
                openAiClient_.call(
                    history, apiKey, model,
                    onReply, onError);
            } else {
                claudeClient_.call(
                    history, apiKey, model,
                    onReply, onError);
            }
        });
}

} // namespace services
