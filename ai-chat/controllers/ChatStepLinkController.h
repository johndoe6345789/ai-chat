#pragma once
/**
 * @file ChatStepLinkController.h
 * @brief Save/retrieve AI context linked to a planner step.
 *
 * GET  /api/chat/step-links/{step_id} — load saved context.
 * POST /api/chat/step-links           — save context.
 */

#include <drogon/HttpController.h>

namespace controllers
{

/**
 * @class ChatStepLinkController
 * @brief Manages chat-to-planner-step links.
 */
class ChatStepLinkController
    : public drogon::HttpController<ChatStepLinkController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        ChatStepLinkController::getLink,
        "/api/chat/step-links/{step_id}",
        drogon::Get,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        ChatStepLinkController::saveLink,
        "/api/chat/step-links",
        drogon::Post,
        "filters::JwtAuthFilter");
    METHOD_LIST_END

    /**
     * @brief Return saved context for a planner step.
     *
     * @param req     HTTP request; step_id from path.
     * @param cb      Response callback.
     * @param stepId  Step identifier from the URL.
     */
    void getLink(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        std::string stepId);

    /**
     * @brief Persist AI context for a planner step.
     *
     * @param req HTTP request; body: {step_id, context}.
     * @param cb  Response callback.
     */
    void saveLink(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
