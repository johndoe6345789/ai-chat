/**
 * @file ai_suggestion_service.cpp
 * @brief Stateless one-shot AI suggestion implementation.
 */

#include "ai-chat/backend/AiSuggestionService.h"

#include <spdlog/spdlog.h>
#include <string>

namespace services
{

namespace
{

constexpr const char* kSystemPrompt =
    "You are an expert startup advisor. Be concise, "
    "practical, and specific. Respond in 2-3 bullet "
    "points.";

constexpr const char* kPromptTemplate =
    "The founder is building a {startup_type} startup "
    "in the {stage} stage. They are working on this "
    "planner step: '{step_title}'. {step_description} "
    "Give 2-3 specific, actionable suggestions for this "
    "step tailored to their startup type and stage. "
    "Focus on the most common mistake founders make at "
    "this stage.";

auto replaceAll(std::string s,
                const std::string& from,
                const std::string& to) -> std::string
{
    std::string::size_type pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

auto buildPrompt(const std::string& title,
                 const std::string& desc,
                 const std::string& type,
                 const std::string& stage) -> std::string
{
    std::string p = kPromptTemplate;
    p = replaceAll(p, "{startup_type}", type);
    p = replaceAll(p, "{stage}",        stage);
    p = replaceAll(p, "{step_title}",   title);
    p = replaceAll(p, "{step_description}", desc);
    return p;
}

} // anonymous namespace

void AiSuggestionService::suggest(
    const std::string& userId,
    const std::string& stepTitle,
    const std::string& stepDescription,
    const std::string& startupType,
    const std::string& stage,
    AiProvider provider,
    const std::string& apiKey,
    const std::string& model,
    Callback onSuccess,
    ErrCallback onError)
{
    spdlog::debug("AiSuggestionService user={} step={}",
                  userId, stepTitle);

    const std::string prompt =
        buildPrompt(stepTitle, stepDescription,
                    startupType, stage);

    json messages = json::array();
    messages.push_back(
        {{"role", "system"}, {"content", kSystemPrompt}});
    messages.push_back(
        {{"role", "user"}, {"content", prompt}});

    auto onReply = [onSuccess](json reply) {
        onSuccess(
            {{"suggestion", reply.value("content", "")}});
    };

    if (provider == AiProvider::OPENAI) {
        openAiClient_.call(
            messages, apiKey, model, onReply, onError);
    } else {
        claudeClient_.call(
            messages, apiKey, model, onReply, onError);
    }
}

} // namespace services
