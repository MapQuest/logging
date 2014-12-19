/*------------------------------------------------------------------------------
 *
 *  Logger which outputs all messages to a file.
 *
 *  Author: matt.amos@mapquest.com
 *
 *  Copyright 2010-1 Mapquest, Inc.  All Rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *-----------------------------------------------------------------------------*/

#include "file_logger.hpp"
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/optional.hpp>
#include <boost/thread/locks.hpp>
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <errno.h>

using boost::optional;
using std::string;
using std::ostringstream;
namespace bt = boost::posix_time;
namespace pt = boost::property_tree;

// a very long time to re-open a file, in seconds: 10 years
#define VERY_LONG_TIME (60L * 60L * 24L * 365L * 10L)

namespace {

logging::logger *create_file_logger(const boost::property_tree::ptree &conf) 
{
   return new logging::file_logger(conf);
}

static const bool registered = logging::register_logger("file", create_file_logger);

}

namespace logging {

file_logger::file_logger(const pt::ptree &conf)
   : m_log_fh(NULL)
{
   optional<string> file_name = conf.get_optional<string>("location");
   optional<unsigned int> reopen_interval = conf.get_optional<unsigned int>("reopen_interval");

   if (file_name) 
   {
      m_file_name = file_name.get();
      m_log_fh = fopen(m_file_name.c_str(), "a");

      if (m_log_fh == NULL)
      {
         throw std::runtime_error((boost::format("Cannot open file at %1% because %2%.") 
                                   % file_name.get() % strerror(errno)).str());
      }
   }
   else 
   {
      throw std::runtime_error("Location parameter not provided for file logging target.");
   }

   if (reopen_interval)
   {
      m_reopen_interval = bt::seconds(reopen_interval.get());
   }
   else
   {
      // we will still re-open the file... in 10 years ;-)
      m_reopen_interval = bt::seconds(VERY_LONG_TIME);
   }
   m_next_reopen = bt::microsec_clock::local_time() + m_reopen_interval;
}

file_logger::~file_logger()
{
   boost::lock_guard<boost::mutex> lock(m_mutex);
   if (m_log_fh != NULL)
   {
      fclose(m_log_fh);
      m_log_fh = NULL;
   }
}

void
file_logger::log(log_level::type level, const std::string &msg)
{
   ostringstream ostr;
   const bt::ptime now = bt::microsec_clock::local_time();

   if (now > m_next_reopen)
   {
      // ye olde venerable double-checked locking...
      boost::lock_guard<boost::mutex> lock(m_mutex);
      if (now > m_next_reopen)
      {
         // reopen the file
         FILE *new_fh = freopen(m_file_name.c_str(), "a", m_log_fh);
         if (new_fh == NULL) 
         {
            // in desperation, squawk something to stderr...
            const string msg = (boost::format("Cannot re-open file at %1% because %2%.") 
                                % m_file_name % strerror(errno)).str();
            std::cerr << msg << std::endl;
            // throw an exception, possibly shutting down the 
            // process.
            throw std::runtime_error(msg);
         }
         else
         {
            m_log_fh = new_fh;
         }
         
         m_next_reopen += m_reopen_interval;
      }
   }

   ostr << now << log::get_utc_offset_string() << " ";
   if (level == log_level::finer) {
      ostr << "[FINER] ";
   } else if (level == log_level::debug) {
      ostr << "[DEBUG] ";
   } else if (level == log_level::info) {
      ostr << "[INFO]  ";
   } else if (level == log_level::warning) {
      ostr << "[WARN]  ";
   } else {
      ostr << "[ERROR] ";
   }
   ostr << msg << "\n";
   fprintf(m_log_fh, "%s", ostr.str().c_str());
   fflush(m_log_fh);
}

} // namespace logging
