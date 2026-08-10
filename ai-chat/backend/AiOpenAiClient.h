#pragma once
/**
 * @file AiOpenAiClient.h
 * @brief HTTP client wrapper for the OpenAI ChatCompletion API.
 *
 * Sends a messages array to OpenAI and returns the assistant
 * reply via callback. The API key is read from the
 * OPENAI_API_KEY environment variable at construction.
 */

#include "ai-chat/backend/AiTypes.h"

#include <string>

namespace services
{

/**
 * @class AiOpenAiClient
 * @brief Wraps the OpenAI /v1/chat/completions endpoint.
 */
class AiOpenAiClient
{
  public:
    /**
     * @brief Construct and load the API key from the
     *        environment.
     */
    AiOpenAiClient();
    ~AiOpenAiClient() = default;

    /**
     * @brief Send @p messages to OpenAI and return the reply.
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
        "gpt-4o-mini";
    static constexpr std::int32_t kMaxTokens = 1024;
};

} // namespace services
