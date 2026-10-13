#pragma once
/**
 * @file AiDraftController.h
 * @brief POST /api/ai/draft-document — AI document
 *        generation for 4 document types.
 */

#include <drogon/HttpController.h>

namespace controllers
{

/**
 * @class AiDraftController
 * @brief Generates pitch decks, investor updates,
 *        NDAs, and job descriptions via AI.
 */
class AiDraftController
    : public drogon::HttpController<AiDraftController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        AiDraftController::draft,
        "/api/ai/draft-document",
        drogon::Post,
        "filters::JwtAuthFilter");
    METHOD_LIST_END

    /**
     * @brief Generate a document using AI.
     *
     * @param req HTTP request; body: document
     *            type + startup metadata.
     * @param cb  Response callback.
     */
    void draft(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
