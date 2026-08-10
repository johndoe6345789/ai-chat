#pragma once
/**
 * @file ChatController.h
 * @brief AI chat endpoints: send message, list history,
 *        and clear history.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class ChatController : public drogon::HttpController<ChatController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ChatController::sendMessage, "/api/chat/messages",
                  drogon::Post, "filters::JwtAuthFilter");
    ADD_METHOD_TO(ChatController::history, "/api/chat/history", drogon::Get,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(ChatController::clearHistory, "/api/chat/history",
                  drogon::Delete, "filters::JwtAuthFilter");
    METHOD_LIST_END

    /** @brief Send a chat message to an AI provider. */
    void sendMessage(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Get paginated chat history. */
    void history(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Delete all chat history for the user. */
    void clearHistory(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
