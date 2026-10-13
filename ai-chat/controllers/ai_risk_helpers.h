#pragma once
/**
 * @file ai_risk_helpers.h
 * @brief Prompt-building helpers for
 *        AiRiskReportController.
 */

#include <nlohmann/json.hpp>
#include <format>
#include <string>

namespace controllers
{

using json = nlohmann::json;

/**
 * @brief Build the risk-report user prompt from
 *        the parsed request body.
 *
 * @param b Parsed JSON body.
 * @return Filled prompt string.
 */
[[nodiscard]] inline auto buildRiskPrompt(
    const json& b) -> std::string
{
    const auto type =
        b.value("startup_type", "startup");
    const auto stage =
        b.value("stage", "early");
    const auto hyp =
        b.value("hypotheses_validated",
                std::int64_t{0});
    const auto burn =
        b.value("burn_months", std::int64_t{0});
    const auto steps =
        b.contains("planner_completed_steps")
        ? static_cast<std::int64_t>(
              b["planner_completed_steps"].size())
        : std::int64_t{0};
    return std::format(
        "The founder is building a {} startup in "
        "the {} stage. They've completed {} planner "
        "steps. Validated {} hypotheses. "
        "Runway: {} months. List 3-5 critical "
        "risks they may be overlooking.",
        type, stage, steps, hyp, burn);
}

} // namespace controllers
