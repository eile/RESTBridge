/* Copyright (c) 2014-2015, Human Brain Project
 *                          Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                          Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
 */

#include <restconnector/RequestHandler.h>
#include <restconnector/RestZeqTranslator.h>

#include <zeq/event.h>
#include <zeq/subscriber.h>
#include <zeq/vocabulary.h>
#include <zeq/publisher.h>

#include <zeq/hbp/vocabulary.h>

#include <lunchbox/uri.h>
#include <lunchbox/log.h>

namespace restconnector
{

RequestHandler::RequestHandler( const std::string& publisherSchema,
                                const std::string& subscriberSchema )
    : subscriber_( new ::zeq::Subscriber( lunchbox::URI( subscriberSchema ) ) )
    , publisher_( new ::zeq::Publisher( lunchbox::URI( publisherSchema ) ) )
    , blocked_( false )
{
    subscriber_->registerHandler( ::zeq::hbp::EVENT_IMAGEJPEG,
                                  boost::bind( &RequestHandler::onImageJPEGEvent_, this, _1 ));
}

RequestHandler::~RequestHandler()
{
}

void RequestHandler::operator() ( const server::request &request, server::response &response )
{
    request_ = request;
    server::string_type ip = source( request );
    RestZeqTranslator restZeqTranslator;

    if ( !restZeqTranslator.isCommandSupported( request.destination ) )
    {
        response = server::response::stock_reply( server::response::bad_request,
                                                  "Command not found." );
    }
    else
    {
        const std::string& method = request.method;
        try
        {
            const ::zeq::Event zeqEvent = restZeqTranslator.translate( request.body );

            if( method == "PUT" )
                processPUT_( zeqEvent );
            else if( method == "GET" )
                processGET_( zeqEvent );
            else if( method == "POST" )
                processPOST_( zeqEvent );

            response = response_;
        }
        catch( const std::runtime_error& e )
        {
            response = server::response::stock_reply( server::response::bad_request,
                                                      e.what() );
        }
    }
}

void RequestHandler::processPUT_( const ::zeq::Event& event )
{
    publisher_->publish( event );
    std::ostringstream data;
    data << "Detected a PUT." <<std::endl;
    response_ = server::response::stock_reply( server::response::ok, data.str() );
}

void RequestHandler::processGET_( const ::zeq::Event& event )
{
    blocked_ = true;
    publisher_->publish( event );

    while( blocked_ )
    {
        while( subscriber_->receive( 0 ) ){}
    }
}

void RequestHandler::processPOST_( const ::zeq::Event& event )
{
    publisher_->publish( event  );
    std::ostringstream data;
    data << "Detected a POST." <<std::endl;
    response_ = server::response::stock_reply( server::response::ok, data.str() );
}

void RequestHandler::onImageJPEGEvent_( const ::zeq::Event& event )
{
    ::zeq::hbp::data::ImageJPEG image( ::zeq::hbp::deserializeImageJPEG( event ) );

    response_ = server::response::stock_reply( server::response::ok,
                                               ::zeq::vocabulary::deserializeJSON( event) );
    blocked_ = false;
}

void RequestHandler::log( server::string_type const &info )
{
    LBERROR << info << std::endl;
}

}