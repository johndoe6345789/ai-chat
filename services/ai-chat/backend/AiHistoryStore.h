#pragma once
/**
 * @file AiHistoryStore.h
 * @brief Persistence layer for AI chat message history.
 *
 * Provides fire-and-forget writes and async reads against
 * the `chat_messages` PostgreSQL table via Drogon ORM.
 */

#include "ai-chat/backend/AiTypes.h"

#include <drogon/orm/DbClient.h>

#include <cstddef>
#include <functional>
#include <string>

namespace services
{

/**
 * @class AiHistoryStore
 * @brief Stores and retrieves chat messages from the database.
 */
class AiHistoryStore
{
  public:
    AiHistoryStore() = default;
    ~AiHistoryStore() = default;

    /**
     * @brief Persist a single chat message row.
     *
     * Fire-and-forget; errors are logged but not propagated.
     *
     * @param userId   Owning user ID.
     * @param role     "user" or "assistant".
     * @param content  Message body.
     * @param provider Provider name string (e.g. "claude").
     * @param model    Model identifier string.
     */
    void storeMessage(const std::string& userId, const std::string& role,
                      const std::string& content, const std::string& provider,
                      const std::string& model);

    /**
     * @brief Load the last @p limit messages for context.
     *
     * Results are returned oldest-first so they can be
     * passed directly to an LLM messages array.
     *
     * @param userId   Owning user ID.
     * @param limit    Maximum number of messages to return.
     * @param callback Receives a JSON array of role/content
     *                 objects, or an empty array on error.
     */
    void loadHistory(const std::string& userId, std::size_t limit,
                     std::function<void(json)> callback);

  private:
    /// Convenience DB accessor.
    [[nodiscard]] static auto db() -> DbClientPtr;
};

} // namespace services
