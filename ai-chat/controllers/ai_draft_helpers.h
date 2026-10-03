#pragma once
/**
 * @file ai_draft_helpers.h
 * @brief Templates and prompt-builder for
 *        AiDraftController.
 */

#include <string>
#include <unordered_map>

namespace controllers
{

/// Document-type → prompt template map.
inline const std::unordered_map<
    std::string, std::string>
    kDraftTemplates = {
        {"pitch_deck",
         "Generate a 12-slide pitch deck outline "
         "for a {startup_type} startup called "
         "'{startup_name}' in the {stage} stage. "
         "Include: Problem, Solution, Market Size, "
         "Business Model, Traction, Team, "
         "Financials, Ask."},
        {"investor_update",
         "Write a one-page monthly investor update "
         "for {startup_name}, a {startup_type} "
         "startup. Include: Highlights, Key "
         "Metrics, Challenges, Next Month Goals, "
         "Asks."},
        {"nda",
         "Generate a short mutual NDA for "
         "{startup_name}. Include: Parties, "
         "Confidential Information definition, "
         "Obligations, Duration (2 years), "
         "Governing Law (England & Wales). "
         "Mark variable fields in [BRACKETS]."},
        {"job_description",
         "Write a job description for an early "
         "hire at {startup_name}, a {startup_type} "
         "startup. Include: Role Summary, What "
         "You'll Do, What We're Looking For, "
         "Why Join Us (equity note)."},
};

/**
 * @brief Replace {placeholder} tokens in a template.
 *
 * @param tmpl  Template string.
 * @param name  Startup name.
 * @param type  Startup type.
 * @param stage Startup stage.
 * @return Filled prompt string.
 */
[[nodiscard]] inline auto fillDraftTemplate(
    std::string tmpl,
    const std::string& name,
    const std::string& type,
    const std::string& stage) -> std::string
{
    auto rep = [](std::string s,
                  const std::string& from,
                  const std::string& to) {
        std::string::size_type p = 0;
        while ((p = s.find(from, p))
               != std::string::npos) {
            s.replace(p, from.size(), to);
            p += to.size();
        }
        return s;
    };
    tmpl = rep(tmpl, "{startup_name}", name);
    tmpl = rep(tmpl, "{startup_type}", type);
    tmpl = rep(tmpl, "{stage}", stage);
    return tmpl;
}

} // namespace controllers
