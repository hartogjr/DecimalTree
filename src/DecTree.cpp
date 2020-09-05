/* BSD 3-Clause License
 *
 * Copyright (c) 2020, Simon de Hartog <simon@dehartog.name>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * vim:set ts=4 sw=4 noet: */

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include "DecTree.h"
#include "Logger.h"
#include "commondefs.h"

namespace SdH {

	DecTree::DecTree()
	: base_(nullptr), nextfree_(0), pages_(0)
	{ }

	DecTree::~DecTree()
	{
		clear();
	}

	void DecTree::clear()
	{
		GRD(mux_);

		if (base_ != nullptr) {
			memset(base_, 0, pages_ * PAGESIZE);
			free(base_);
			base_ = nullptr;
			pages_ = 0;
		}
	}

	uint64_t extra_(const uint8_t bytes_i)
	{
		void *newbase = nullptr;
		uint64_t offset;
		if (nextfree_ + bytes_i > pages_ * PAGESIZE) {
			newbase = realloc( (pages_ + 1) * PAGESIZE);
			FCET(newbase != nullptr, std::bad_alloc, "Unable to allocate an extra memory page ({} bytes already allocated)",
				pages_ * PAGESIZE);
			pages_++;
			base_ = newbase_;
		}
		offset = nextfree_;
		memset(base_ + nextfree_, 0, bytes_i);
		nextfree_ += bytes_i;
		return offset;
	}

	const uint64_t DecTree::operator()(const std::string & number_i)
	{
	}

	void DecTree::operator()(const std::string & number_i, const uint64_t destination_i)
	{
		FCET(number_i.size(), std::invalid_argument, "Number to store is empty");
		size_t pos;
		pos = number.find_first_not_of("0123456789");
		FCET(pos == std::string::npos,
			std::invalid_argument,
			"Number \"{}\" to set contains at least one non-digit at position {}",
			number_i, pos
		);
		uint64_t offset = 0;
		bool leaf = false;
		uint64_t *val = nullptr;

		GRD(mux_);
		if (base_ == nullptr) {
			base_ = malloc(PAGESIZE);
			if (base_ == nullptr)

			nextfree_ = 0;
			pages_ = 1;
			offset = newlist_();
		}

		leaf = false;
		for (uint16_t i = 0; i < number_i.size(); i++) {
			if (!leaf) {
				val = base_ + offset + (number_[i]&0xF);
				if (!ISVALID(*val)) {
					offset = newlist_();
					// TODO
				}
				// TODO
			}
			// TODO
		}

	}

} // SdH namespace
