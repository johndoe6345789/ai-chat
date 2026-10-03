#pragma once
/**
 * @file AiSuggestionService.h
 * @brief Stateless one-shot AI suggestion for a planner step.
 *
 * Builds a filled prompt from suggestion-prompt.json, sends it
 * to the selected AI provider, and returns 2-3 bullet points
 * via callback. No history is stored.
 */

#include "ai-chat/backend/AiClaudeClient.h"
#include "ai-chat/backend/AiOpenAiClient.h"
#include "ai-chat/backend/AiTypes.h"

#include <string>

namespace services
{

/**
 * @class AiSuggestionService
 * @brief Sends a one-shot planner-step prompt to an AI provider.
 */
class AiSuggestionService
{
  public:
    AiSuggestionService() = default;
    ~AiSuggestionService() = default;

    /**
     * @brief Request AI suggestions for a specific planner step.
     *
     * @param userId          Authenticated user ID.
     * @param stepTitle       Title of the planner step.
     * @param stepDescription Optional description of the step.
     * @param startupType     Type/category of the startup.
     * @param stage           Startup stage (e.g. "early").
     * @param provider        Which AI backend to call.
     * @param apiKey          Resolved API key (empty = env).
     * @param model           Model override (empty = default).
     * @param onSuccess       Called with suggestion JSON.
     * @param onError         Called on failure.
     */
    void suggest(
        const std::string& userId,
        const std::string& stepTitle,
        const std::string& stepDescription,
        const std::string& startupType,
        const std::string& stage,
        AiProvider provider,
        const std::string& apiKey,
        const std::string& model,
        Callback onSuccess,
        ErrCallback onError);

  private:
    AiClaudeClient claudeClient_;
    AiOpenAiClient openAiClient_;
};

} // namespace services
