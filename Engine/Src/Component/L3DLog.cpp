#include "stdafx.h"
#include <timeapi.h>

#include "spdlog/sinks/basic_file_sink.h"
#include "L3DLog.h"

L3DLog::L3DLog()
{
    char szFileName[MAX_PATH];
    auto time = std::time(nullptr);
    std::strftime(szFileName, MAX_PATH, "logs/engine-%F-%H-%M-%S.txt", std::localtime(&time));

    auto logger = spdlog::basic_logger_st("engine", szFileName);
    spdlog::set_default_logger(logger);
}

L3DLog::~L3DLog()
{
    spdlog::default_logger()->flush();
}
