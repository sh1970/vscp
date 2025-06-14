// vscp_client_udp.cpp
//
// tcp/ip client communication classes.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version
// 2 of the License, or (at your option) any later version.
//
// This file is part of the VSCP (https://www.vscp.org)
//
// Copyright:  (C) 2007-2025
// Ake Hedman, the VSCP project, <info@vscp.org>
//
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this file see the file COPYING.  If not, write to
// the Free Software Foundation, 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//

#ifdef WIN32
#include <pch.h>
#endif

#include <mustache.hpp>
#include <nlohmann/json.hpp> // Needs C++11  -std=c++11

#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "vscphelper.h"
#include "vscp-client-udp.h"

///////////////////////////////////////////////////////////////////////////////
// CTor
//

vscpClientUdp::vscpClientUdp() : CVscpClient()
{
    m_type = CVscpClient::connType::UDP;
}

///////////////////////////////////////////////////////////////////////////////
// DTor
//

vscpClientUdp::~vscpClientUdp()
{
    ;
}

///////////////////////////////////////////////////////////////////////////////
// getConfigAsJson
//

std::string vscpClientUdp::getConfigAsJson(void) 
{
    std::string rv;

    return rv;
}


///////////////////////////////////////////////////////////////////////////////
// initFromJson
//

bool vscpClientUdp::initFromJson(const std::string& /*config*/)
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// connect
//

int vscpClientUdp::connect(void)
{
    return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// disconnect
//

int vscpClientUdp::disconnect(void)
{
    return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// isConnected
//

bool vscpClientUdp::isConnected(void)
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// send
//

int vscpClientUdp::send(vscpEvent & /*ev*/)
{
    return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// send
//

int vscpClientUdp::send(vscpEventEx & /*ex*/)
{
    return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// send
//

int
vscpClientUdp::send(canalMsg &msg)
{
  vscpEventEx ex;

  uint8_t guid[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  if (vscp_convertCanalToEventEx(&ex, &msg, guid)) {
    return VSCP_ERROR_INVALID_FRAME;
  }

  return send(ex);
}

///////////////////////////////////////////////////////////////////////////////
// receive
//

int vscpClientUdp::receive(vscpEvent & /*ev*/)
{
    return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// receive
//

int vscpClientUdp::receive(vscpEventEx & /*ex*/)
{
    return VSCP_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// receive
//

int  
vscpClientUdp::receive(canalMsg &msg)
{
  int rv;
  vscpEventEx ex;

  if (VSCP_ERROR_SUCCESS != (rv = receive(ex))) {
    return rv;
  }

  if (!vscp_convertEventExToCanal(&msg, &ex)) {
    return VSCP_ERROR_INVALID_FRAME;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// receiveBlocking
//

int
vscpClientUdp::receiveBlocking(vscpEvent &ev, long timeout)
{
  // if (-1 == vscp_sem_wait(&m_semReceiveQueue, timeout)) {
  //   if (errno == ETIMEDOUT) {
  //     return VSCP_ERROR_TIMEOUT;
  //   }
  //   else {
  //     return VSCP_ERROR_ERROR;
  //   }
  // }

  return receive(ev);
}

///////////////////////////////////////////////////////////////////////////////
// receiveBlocking
//

int
vscpClientUdp::receiveBlocking(vscpEventEx &ex, long timeout)
{
  // if (-1 == vscp_sem_wait(&m_semReceiveQueue, timeout)) {
  //   if (errno == ETIMEDOUT) {
  //     return VSCP_ERROR_TIMEOUT;
  //   }
  //   else {
  //     return VSCP_ERROR_ERROR;
  //   }
  // }

  return receive(ex);
}

///////////////////////////////////////////////////////////////////////////////
// receiveBlocking
//

int
vscpClientUdp::receiveBlocking(canalMsg &msg, long timeout)
{
  // if (-1 == vscp_sem_wait(&m_semReceiveQueue, timeout)) {
  //   if (errno == ETIMEDOUT) {
  //     return VSCP_ERROR_TIMEOUT;
  //   }
  //   else {
  //     return VSCP_ERROR_ERROR;
  //   }
  // }

  return receive(msg);
}

//////////////////////////////////////////////////////////////////////////////
// setfilter
//

int vscpClientUdp::setfilter(vscpEventFilter & /*filter*/)
{
    return VSCP_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// getcount
//

int vscpClientUdp::getcount(uint16_t *pcount)
{
    if (NULL == pcount ) return VSCP_ERROR_INVALID_POINTER;
    *pcount = 0;
    return VSCP_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// clear
//

int vscpClientUdp::clear()
{
    return VSCP_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// getversion
//

int vscpClientUdp::getversion(uint8_t *pmajor,
                                uint8_t *pminor,
                                uint8_t *prelease,
                                uint8_t *pbuild)
{
    if (NULL == pmajor ) return VSCP_ERROR_INVALID_POINTER;
    if (NULL == pminor ) return VSCP_ERROR_INVALID_POINTER;
    if (NULL == prelease ) return VSCP_ERROR_INVALID_POINTER;
    if (NULL == pbuild ) return VSCP_ERROR_INVALID_POINTER;
    return VSCP_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// getinterfaces
//

int vscpClientUdp::getinterfaces(std::deque<std::string> & /*iflist*/)
{
    return VSCP_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// getwcyd
//

int vscpClientUdp::getwcyd(uint64_t & /*wcyd*/)
{
    return VSCP_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// setConnectionTimeout
//

void vscpClientUdp::setConnectionTimeout(uint32_t /*timeout*/)
{
    ;
}

//////////////////////////////////////////////////////////////////////////////
// getConnectionTimeout
//

uint32_t vscpClientUdp::getConnectionTimeout(void)
{
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// setResponseTimeout
//

void vscpClientUdp::setResponseTimeout(uint32_t /*timeout*/)
{
    ;
}

//////////////////////////////////////////////////////////////////////////////
// getResponseTimeout
//

uint32_t vscpClientUdp::getResponseTimeout(void)
{
    return 0;
}