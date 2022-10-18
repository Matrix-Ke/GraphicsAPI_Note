#pragma once
#include "Core/GlobalContext.h"

#include <spdlog/spdlog.h>

#include <cstdint>
#include <stdexcept>

namespace Matrix
{
	//g_runtime_glo
	class LogSystem final
	{
	public:
		enum class LogLevel : uint8_t
		{
			debug,
			info,
			warn,
			error,
			fatal
		};

	public:
		LogSystem();
		~LogSystem();

		template<typename... TARGS>
		void log(LogLevel level, TARGS&&... args)
		{
			switch (level)
			{
			case LogLevel::debug:
				m_logger->debug(std::forward<TARGS>(args)...);
				break;
			case LogLevel::info:
				m_logger->info(std::forward<TARGS>(args)...);
				break;
			case LogLevel::warn:
				m_logger->warn(std::forward<TARGS>(args)...);
				break;
			case LogLevel::error:
				m_logger->error(std::forward<TARGS>(args)...);
				break;
			case LogLevel::fatal:
				m_logger->critical(std::forward<TARGS>(args)...);
				fatalCallback(std::forward<TARGS>(args)...);
				break;
			default:
				break;
			}
		}

		template<typename... TARGS>
		void fatalCallback(TARGS&&... args)
		{
			const std::string format_str = fmt::format(std::forward<TARGS>(args)...);
			throw std::runtime_error(format_str);
		}

	private:
		std::shared_ptr<spdlog::logger> m_logger;
	};

} 

#define LOG_HELPER(LOG_LEVEL, ...)  g_runtime_global_context.m_logger_system->log(LOG_LEVEL, "[" + std::string(__FUNCTION__) + "] " + __VA_ARGS__);

#define LOG_DEBUG(...) LOG_HELPER(LogSystem::LogLevel::debug, __VA_ARGS__);

#define LOG_INFO(...) LOG_HELPER(LogSystem::LogLevel::info, __VA_ARGS__);

#define LOG_WARN(...) LOG_HELPER(LogSystem::LogLevel::warn, __VA_ARGS__);

#define LOG_ERROR(...) LOG_HELPER(LogSystem::LogLevel::error, __VA_ARGS__);

#define LOG_FATAL(...) LOG_HELPER(LogSystem::LogLevel::fatal, __VA_ARGS__);



