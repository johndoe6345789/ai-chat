#pragma once
/**
 * @file AiRiskReportController.h
 * @brief POST /api/ai/risk-report — one-shot plan
 *        risk analysis.
 */

#include <drogon/HttpController.h>

namespace controllers
{

/**
 * @class AiRiskReportController
 * @brief Analyses the founder's full plan state
 *        and returns 3-5 blind spots.
 */
class AiRiskReportController
    : public drogon::HttpController<
          AiRiskReportController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        AiRiskReportController::report,
        "/api/ai/risk-report",
        drogon::Post,
        "filters::JwtAuthFilter");
    METHOD_LIST_END

    /**
     * @brief Generate a risk report for a founder's plan.
     *
     * @param req HTTP request; body: plan state JSON.
     * @param cb  Response callback.
     */
    void report(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
