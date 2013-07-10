/*------------------------------------------------------------------------------
 *
 *  Logger which doesn't output any messages.
 *
 *  Author: matt.amos@mapquest.com
 *
 *  Copyright 2010-2 Mapquest, Inc.  All Rights reserved.
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

#ifndef LOGGING_NULL_LOGGER_HPP
#define LOGGING_NULL_LOGGER_HPP

#include "logger.hpp"

namespace rendermq {

/* a useful little utility - a logger which outputs nothing.
 * this is useful as a sort of optional<logger> when it's 
 * easier for readability not to have optional<>s all over 
 * the place.
 */
class null_logger
   : public logger
{
public:
   void log(log_level::type, const std::string &);
   virtual ~null_logger();
};

} // namespace rendermq

#endif /* LOGGING_NULL_LOGGER_HPP */
