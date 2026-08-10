#pragma once
/**
 * @file AiService.h
 * @brief Orchestrator for AI chat: history, routing, storage.
 *
 * Composes AiHistoryStore, AiClaudeClient, and AiOpenAiClient
 * to handle a complete chat turn: load history, send to the
 * selected provider, and persist the exchange.
 */

#include "ai-chat/backend/AiClaudeClient.h"
#include "ai-chat/backend/AiHistoryStore.h"
#include "ai-chat/backend/AiOpenAiClient.h"
#include "ai-chat/backend/AiTypes.h"

namespace services
{

/**
 * @class AiService
 * @brief Sends chat messages to an LLM and stores the
 *        conversation.
 */
class AiService
{
  public:
    AiService();
    ~AiService() = default;

    /**
     * @brief Send a user message to the AI provider.
     *
     * 1. Stores the user message.
     * 2. Loads recent conversation history.
     * 3. Sends the prompt to Claude or OpenAI.
     * 4. Persists the assistant reply.
     * 5. Returns the assistant reply via @p onSuccess.
     *
     * @param userId    Authenticated user ID.
     * @param message   User's plain-text message.
     * @param provider  Which backend to use.
     * @param apiKey    Resolved API key (empty = env).
     * @param model     Model override (empty = default).
     * @param onSuccess Callback with AiResponse JSON.
     * @param onError   Callback on failure.
     */
    void chat(const std::string& userId,
              const std::string& message,
              AiProvider provider,
              const std::string& apiKey,
              const std::string& model,
              Callback onSuccess,
              ErrCallback onError);

  private:
    AiHistoryStore historyStore_;
    AiClaudeClient claudeClient_;
    AiOpenAiClient openAiClient_;

    /// Max history messages to include as context.
    static constexpr std::size_t kHistoryLimit = 20;
};

} // namespace services
