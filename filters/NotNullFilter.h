/**
 *
 *  NotNullFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;


class NotNullFilter : public HttpFilter<NotNullFilter>
{
  public:
    NotNullFilter() {}
    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};

