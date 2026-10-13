#pragma once
/**
 * @file AiSuggestionController.h
 * @brief POST /api/ai/step-suggestion — one-shot planner step
 *        AI suggestions.
 */

#include <drogon/HttpController.h>

namespace controllers
{

/**
 * @class AiSuggestionController
 * @brief Handles the step-suggestion AI endpoint.
 */
class AiSuggestionController
    : public drogon::HttpController<AiSuggestionController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AiSuggestionController::suggest,
                  "/api/ai/step-suggestion",
                  drogon::Post,
                  "filters::JwtAuthFilter");
    METHOD_LIST_END

    /**
     * @brief Return 2-3 AI suggestions for a planner step.
     *
     * @param req HTTP request containing step context JSON.
     * @param cb  Response callback.
     */
    void suggest(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
