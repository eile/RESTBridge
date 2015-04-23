/* Copyright (c) 2014-2015, Human Brain Project
 *                          Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
 */

#include "RestBridge.h"
#include "detail/RequestHandler.h"

#include <lunchbox/debug.h>
#include <lunchbox/log.h>
#include <lunchbox/monitor.h>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>

namespace restbridge
{

static const std::string PUBLISHER_SCHEMA_SUFFIX = "cmd://";
static const std::string SUBSCRIBER_SCHEMA_SUFFIX = "resp://";

namespace detail
{
class RestBridge
{
public:
    RestBridge( const std::string& hostname, const uint16_t port )
        : serverRunning_( false )
        , hostname_( hostname )
        , port_( port )
    {
    }

    ~RestBridge()
    {
        if( serverRunning_ )
        {
            try
            {
                stop();
            }
            catch( ... ) {}
        }
    }

    void run( const std::string& schema )
    {
        const std::string publisherSchema = schema + PUBLISHER_SCHEMA_SUFFIX;
        const std::string subscriberSchema = schema + SUBSCRIBER_SCHEMA_SUFFIX;
        RequestHandler handler( publisherSchema, subscriberSchema );

        server::options options( handler );
        std::stringstream port;
        port << port_;
        httpServer_.reset( new server( options.address( hostname_ ).port( port.str( ))));

        LBINFO << "HTTP Server runnning on " << hostname_ << ":" << port_ << std::endl;
        LBINFO << "ZeroEq Publisher Schema : " << publisherSchema << std::endl;
        LBINFO << "ZeroEq Subscriber Schema: " << subscriberSchema << std::endl;
        serverRunning_ = true;
        httpServer_->run();
    }

    void stop()
    {
        if( !thread_ || !serverRunning_ )
            LBTHROW( std::runtime_error( "HTTP server is not running, cannot stop it" ));
        httpServer_->stop();
        thread_->join();
        serverRunning_ = false;
        LBINFO << "HTTP Server on " << hostname_ << ":" << port_
               << " is stopped" << std::endl;
    }

    boost::scoped_ptr< boost::thread > thread_;
    boost::scoped_ptr< server > httpServer_;
    lunchbox::Monitor< bool > serverRunning_;
    std::string hostname_;
    uint16_t port_;
};
}

RestBridge::RestBridge( const std::string& hostname, const uint16_t port )
    : _impl( new detail::RestBridge( hostname, port ))
{
}

RestBridge::~RestBridge()
{
    delete _impl;
}

void RestBridge::run( const std::string& schema )
{
    if( _impl->thread_ )
        LBTHROW( std::runtime_error( "HTTP server is already running" ));
    _impl->thread_.reset( new boost::thread(
        boost::bind( &detail::RestBridge::run, _impl, schema )));
    if( ! _impl->serverRunning_.timedWaitEQ( true, 2000 ))
        LBTHROW( std::runtime_error( "HTTP server could not be started" ));
}

void RestBridge::stop()
{
    _impl->stop();
}

}