/* Copyright (c) 2020 Simon de Hartog <simon@dehartog.name>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

vim:set ts=4 sw=4 noexpandtab: */

#include <iostream>
#include <cstdarg>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <stdarg.h>
#include <time.h>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <sys/time.h>
#include "Logger.h"

namespace Fs2a {

	Logger::Logger()
		: maxlevel_(Logger::debug), stream_(nullptr), strip_(0), syslog_(false)
	{
		levels_[error]   = "ERROR";
		levels_[warning] = "WARNING";
		levels_[notice]  = "NOTICE";
		levels_[info]    = "INFO";
		levels_[debug]   = "DEBUG";
	}

	Logger::~Logger()
	{
		GRD(mymux_);

		if (syslog_) {
			closelog();
			syslog_ = false;
		} else stream_ = nullptr;
	}

	std::unique_ptr<std::string> Logger::log(
		const std::string & file_i,
		const size_t & line_i,
		const loglevel_t priority_i,
		const std::string & msg_i
	)
	{
		struct timeval tv;      // Time value storage
		std::string le;         // Log Entry containing final result

		if (priority_i > maxlevel_) return std::unique_ptr<std::string>();

		gettimeofday(&tv, nullptr);

		le = fmt::format("{:%T}.{:06d} [{}] {}:{} ",
			fmt::localtime(tv.tv_sec),
			tv.tv_usec,
			std::this_thread::get_id(),
			file_i.substr(strip_),
			line_i
		);

		if (!syslog_) {
			le += levels_[priority_i];
			le += " ";
		}

		le += msg_i;

		if (syslog_) {
			::syslog(priority_i, "%s", le.c_str());
		}
		else {
			if (stream_ == nullptr) {
				throw std::logic_error("Asked to log to stream, but stream is NULL");
			}
			*stream_ << le << std::endl;
		}

		return std::unique_ptr<std::string>(new std::string(le));
	}

	void Logger::stream(std::ostream * stream_i, const size_t strip_i)
	{
		if (stream_i == nullptr) {
			throw std::invalid_argument("Unable to write log output to NULL stream pointer");
		}

		GRD(mymux_);

		if (syslog_) {
			closelog();
			syslog_ = false;
		}

		strip_ = strip_i;

		stream_ = stream_i;
	}

	bool Logger::syslog(const std::string ident_i, const int facility_i, const size_t strip_i)
	{
		GRD(mymux_);

		if (syslog_) return false;

		ident_ = ident_i;
		strip_ = strip_i;

		openlog(ident_.c_str(), LOG_CONS | LOG_NDELAY | LOG_PID, facility_i);
		stream_ = nullptr;
		syslog_ = true;
		return true;
	}

} // Fs2a namespace
