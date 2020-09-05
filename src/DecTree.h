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
 * vim:set ts=4 sw=4 noet tw=120: */

#pragma once

#include <cstdint>
#include <mutex>

#define ISVALID(x)     (x & UINT64_C(0x01))
#define POINTS2LEAF(x) (x & UINT64_C(0x02))

namespace SdH {

	class DecTree
	{
		private:
		/// Copy construction not allowed
		DecTree(const DecTree * obj_i) = delete;

		/// Assignment construction not allowed
		DecTree & operator=(const DecTree & obj_i) = delete;

		protected:
		/// Base address of data
		void *base_;

		/// Mutex to prevent simultaneous modifications
		std::mutex mux_;

		/// Next free byte in allocated memory
		uint64_t nextfree_;

		/// Number of memory pages allocated
		uint32_t pages_;

		/** Reset a block of memory, possibly allocating more pages of
		 * necessary.
		 * @param bytes_i Number of bytes to clear
		 * @returns Offset of block, relative to base. */
		uint64_t extra_(const uint8_t bytes_i);

		/** Create a new leaf in memory, possibly allocating more pages if
		 * necessary.
		 * @returns Offset of new leaf, relative to base. */
		inline uint64_t newleaf_() { return extra_(sizeof(uint64_t)); }

		/** Create a new list in memory, possibly allocating more pages if
		 * necessary.
		 * @returns Offset in bytes of new list, relative to base. */
		inline uint64_t newlist_() { return extra_(sizeof(uint64_t)*11); }

		public:
		/// Constructor
		DecTree();

		/// Destructor
		~DecTree();

		/** Clear the entire database. This method will cause segfaults when
		 * reads are still being executed. */
		void clear();

		/** Lookup a destination for a given number.
		 * @param number_i Number to lookup.
		 * @returns Found destination, or 0 if not found.
		 * @throws std::invalid_argument if @p number_i does not consist of
		 * only digits in the range 0 through 9. */
		const uint64_t operator()(const std::string & number_i) const;

		/** Set a destination for a number (range).
		 * This method creates decimal trees and allocates memory as
		 * necessary. It also replaces possible existing entries.
		 * @param number_i The number (range) to set.
		 * @param destination_i The destination to set for this number (range).
		 * @throws std::invalid_argument if @p number_i does not consist of
		 * only digits in the range 0 through 9. */
		void operator()(const std::string & number_i, const uint64_t destination_i);
	};

} // SdH namespace
