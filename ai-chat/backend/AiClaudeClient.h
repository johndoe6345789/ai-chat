#pragma once
/**
 * @file AiClaudeClient.h
 * @brief HTTP client wrapper for the Anthropic Claude API.
 *
 * Sends a messages array to Claude and returns the assistant
 * reply via callback. The API key is read from the
 * ANTHROPIC_API_KEY environment variable at construction.
 */

#include "ai-chat/backend/AiTypes.h"

#include <string>

namespace services
{

/**
 * @class AiClaudeClient
 * @brief Wraps the Anthropic /v1/messages endpoint.
 */
class AiClaudeClient
{
  public:
    /**
     * @brief Construct and load the API key from the
     *        environment.
     */
    AiClaudeClient();
    ~AiClaudeClient() = default;

    /**
     * @brief Send @p messages to Claude and return the reply.
     *
     * @param messages  JSON array of role/content objects.
     * @param onSuccess Called with content/model/tokensUsed
     *                  JSON on success.
     * @param onError   Called with status + message on
     *                  failure.
     */
    void call(const json& messages,
              Callback onSuccess,
              ErrCallback onError);

    /**
     * @brief Send with an explicit API key and model.
     *
     * @param messages  JSON array of role/content objects.
     * @param apiKey    API key to use (overrides env).
     * @param model     Model to use (empty = default).
     * @param onSuccess Called with result JSON.
     * @param onError   Called on failure.
     */
    void call(const json& messages,
              const std::string& apiKey,
              const std::string& model,
              Callback onSuccess,
              ErrCallback onError);

  private:
    std::string apiKey_;

    static constexpr const char* kModel =
        "claude-haiku-4-5-20251001";
    static constexpr std::int32_t kMaxTokens = 1024;
};

} // namespace services
