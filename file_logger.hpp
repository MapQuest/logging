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

#ifndef LOGGING_FILE_LOGGER_HPP
#define LOGGING_FILE_LOGGER_HPP

#include "logger.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>
#include <cstdio>

namespace rendermq {

/* logs everything to a file.
 */
class file_logger
   : public logger
{
public:
   explicit file_logger(const boost::property_tree::ptree &);
   virtual ~file_logger();

   void log(log_level::type, const std::string &);

private:
   // the file that gets logged to.
   FILE *m_log_fh;

   // file name to log to.
   std::string m_file_name;

   // the time that the log file should be next checked.
   boost::posix_time::ptime m_next_reopen;

   // time between re-opens of the log file. this supports
   // log rotation.
   boost::posix_time::time_duration m_reopen_interval;

   // mutex used to serialize changes to m_next_reopen and
   // m_log_fh.
   boost::mutex m_mutex;
};

} // namespace rendermq

#endif /* LOGGING_FILE_LOGGER_HPP */
