/* soapwsddProxy.h
   Generated by gSOAP 2.8.122 for onvif.h

gSOAP XML Web services tools
Copyright (C) 2000-2021, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#ifndef soapwsddProxy_H
#define soapwsddProxy_H
#include "soapH.h"

    class SOAP_CMAC wsddProxy {
      public:
        /// Context to manage proxy IO and data
        struct soap *soap;
        /// flag indicating that this context is owned by this proxy and should be deleted by the destructor
        bool soap_own;
        /// Endpoint URL of service 'wsddProxy' (change as needed)
        const char *soap_endpoint;
        /// Variables globally declared in onvif.h, if any
        /// Construct a proxy with new managing context
        wsddProxy();
        /// Copy constructor
        wsddProxy(const wsddProxy& rhs);
        /// Construct proxy given a shared managing context
        wsddProxy(struct soap*);
        /// Construct proxy given a shared managing context and endpoint URL
        wsddProxy(struct soap*, const char *soap_endpoint_url);
        /// Constructor taking an endpoint URL
        wsddProxy(const char *soap_endpoint_url);
        /// Constructor taking input and output mode flags for the new managing context
        wsddProxy(soap_mode iomode);
        /// Constructor taking endpoint URL and input and output mode flags for the new managing context
        wsddProxy(const char *soap_endpoint_url, soap_mode iomode);
        /// Constructor taking input and output mode flags for the new managing context
        wsddProxy(soap_mode imode, soap_mode omode);
        /// Destructor deletes deserialized data and its managing context, when the context was allocated by the constructor
        virtual ~wsddProxy();
        /// Initializer used by constructors
        virtual void wsddProxy_init(soap_mode imode, soap_mode omode);
        /// Return a copy that has a new managing context with the same engine state
        virtual wsddProxy *copy();
        /// Copy assignment
        wsddProxy& operator=(const wsddProxy&);
        /// Delete all deserialized data (uses soap_destroy() and soap_end())
        virtual void destroy();
        /// Delete all deserialized data and reset to default
        virtual void reset();
        /// Disables and removes SOAP Header from message by setting soap->header = NULL
        virtual void soap_noheader();
        /// Add SOAP Header to message
        virtual void soap_header(char *wsa__MessageID, struct wsa__Relationship *wsa__RelatesTo, struct wsa__EndpointReferenceType *wsa__From, struct wsa__EndpointReferenceType *wsa__ReplyTo, struct wsa__EndpointReferenceType *wsa__FaultTo, char *wsa__To, char *wsa__Action, struct wsdd__AppSequenceType *wsdd__AppSequence, char *wsa5__MessageID, struct wsa5__RelatesToType *wsa5__RelatesTo, struct wsa5__EndpointReferenceType *wsa5__From, struct wsa5__EndpointReferenceType *wsa5__ReplyTo, struct wsa5__EndpointReferenceType *wsa5__FaultTo, char *wsa5__To, char *wsa5__Action, struct chan__ChannelInstanceType *chan__ChannelInstance);
        /// Get SOAP Header structure (i.e. soap->header, which is NULL when absent)
        virtual ::SOAP_ENV__Header *soap_header();
        /// Get SOAP Fault structure (i.e. soap->fault, which is NULL when absent)
        virtual ::SOAP_ENV__Fault *soap_fault();
        /// Get SOAP Fault subcode QName string (NULL when absent)
        virtual const char *soap_fault_subcode();
        /// Get SOAP Fault string/reason (NULL when absent)
        virtual const char *soap_fault_string();
        /// Get SOAP Fault detail XML string (NULL when absent)
        virtual const char *soap_fault_detail();
        /// Close connection (normally automatic, except for send_X ops)
        virtual int soap_close_socket();
        /// Force close connection (can kill a thread blocked on IO)
        virtual int soap_force_close_socket();
        /// Print fault
        virtual void soap_print_fault(FILE*);
    #ifndef WITH_LEAN
    #ifndef WITH_COMPAT
        /// Print fault to stream
        virtual void soap_stream_fault(std::ostream&);
    #endif
        /// Write fault to buffer
        virtual char *soap_sprint_fault(char *buf, size_t len);
    #endif
        //
        /// Web service one-way synchronous send operation 'Hello' to the default endpoint with the default SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int Hello(struct wsdd__HelloType *wsdd__Hello) { return this->Hello(NULL, NULL, wsdd__Hello); }
        /// Web service one-way synchronous send operation 'Hello' to the specified endpoint and SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int Hello(const char *soap_endpoint_url, const char *soap_action, struct wsdd__HelloType *wsdd__Hello) { return (this->send_Hello(soap_endpoint_url, soap_action, wsdd__Hello) || soap_recv_empty_response(this->soap)) ? this->soap->error : SOAP_OK; }
        /// Web service one-way asynchronous send operation 'send_Hello' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int send_Hello(struct wsdd__HelloType *wsdd__Hello) { return this->send_Hello(NULL, NULL, wsdd__Hello); }
        /// Web service one-way asynchronous send operation 'send_Hello' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_Hello(const char *soap_endpoint_url, const char *soap_action, struct wsdd__HelloType *wsdd__Hello);
        /// Web service one-way asynchronous receive operation 'recv_Hello', returns SOAP_OK or error code;
        virtual int recv_Hello(struct __wsdd__Hello&);
        /// Web service asynchronous receive of HTTP Accept acknowledgment for one-way asynchronous send operation 'send_Hello', returns SOAP_OK or error code
        virtual int recv_Hello_empty_response() { return soap_recv_empty_response(this->soap); }
        //
        /// Web service one-way synchronous send operation 'Bye' to the default endpoint with the default SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int Bye(struct wsdd__ByeType *wsdd__Bye) { return this->Bye(NULL, NULL, wsdd__Bye); }
        /// Web service one-way synchronous send operation 'Bye' to the specified endpoint and SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int Bye(const char *soap_endpoint_url, const char *soap_action, struct wsdd__ByeType *wsdd__Bye) { return (this->send_Bye(soap_endpoint_url, soap_action, wsdd__Bye) || soap_recv_empty_response(this->soap)) ? this->soap->error : SOAP_OK; }
        /// Web service one-way asynchronous send operation 'send_Bye' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int send_Bye(struct wsdd__ByeType *wsdd__Bye) { return this->send_Bye(NULL, NULL, wsdd__Bye); }
        /// Web service one-way asynchronous send operation 'send_Bye' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_Bye(const char *soap_endpoint_url, const char *soap_action, struct wsdd__ByeType *wsdd__Bye);
        /// Web service one-way asynchronous receive operation 'recv_Bye', returns SOAP_OK or error code;
        virtual int recv_Bye(struct __wsdd__Bye&);
        /// Web service asynchronous receive of HTTP Accept acknowledgment for one-way asynchronous send operation 'send_Bye', returns SOAP_OK or error code
        virtual int recv_Bye_empty_response() { return soap_recv_empty_response(this->soap); }
        //
        /// Web service one-way synchronous send operation 'Probe' to the default endpoint with the default SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int Probe(struct wsdd__ProbeType *wsdd__Probe) { return this->Probe(NULL, NULL, wsdd__Probe); }
        /// Web service one-way synchronous send operation 'Probe' to the specified endpoint and SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int Probe(const char *soap_endpoint_url, const char *soap_action, struct wsdd__ProbeType *wsdd__Probe) { return (this->send_Probe(soap_endpoint_url, soap_action, wsdd__Probe) || soap_recv_empty_response(this->soap)) ? this->soap->error : SOAP_OK; }
        /// Web service one-way asynchronous send operation 'send_Probe' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int send_Probe(struct wsdd__ProbeType *wsdd__Probe) { return this->send_Probe(NULL, NULL, wsdd__Probe); }
        /// Web service one-way asynchronous send operation 'send_Probe' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_Probe(const char *soap_endpoint_url, const char *soap_action, struct wsdd__ProbeType *wsdd__Probe);
        /// Web service one-way asynchronous receive operation 'recv_Probe', returns SOAP_OK or error code;
        virtual int recv_Probe(struct __wsdd__Probe&);
        /// Web service asynchronous receive of HTTP Accept acknowledgment for one-way asynchronous send operation 'send_Probe', returns SOAP_OK or error code
        virtual int recv_Probe_empty_response() { return soap_recv_empty_response(this->soap); }
        //
        /// Web service one-way synchronous send operation 'ProbeMatches' to the default endpoint with the default SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int ProbeMatches(struct wsdd__ProbeMatchesType *wsdd__ProbeMatches) { return this->ProbeMatches(NULL, NULL, wsdd__ProbeMatches); }
        /// Web service one-way synchronous send operation 'ProbeMatches' to the specified endpoint and SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int ProbeMatches(const char *soap_endpoint_url, const char *soap_action, struct wsdd__ProbeMatchesType *wsdd__ProbeMatches) { return (this->send_ProbeMatches(soap_endpoint_url, soap_action, wsdd__ProbeMatches) || soap_recv_empty_response(this->soap)) ? this->soap->error : SOAP_OK; }
        /// Web service one-way asynchronous send operation 'send_ProbeMatches' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int send_ProbeMatches(struct wsdd__ProbeMatchesType *wsdd__ProbeMatches) { return this->send_ProbeMatches(NULL, NULL, wsdd__ProbeMatches); }
        /// Web service one-way asynchronous send operation 'send_ProbeMatches' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_ProbeMatches(const char *soap_endpoint_url, const char *soap_action, struct wsdd__ProbeMatchesType *wsdd__ProbeMatches);
        /// Web service one-way asynchronous receive operation 'recv_ProbeMatches', returns SOAP_OK or error code;
        virtual int recv_ProbeMatches(struct __wsdd__ProbeMatches&);
        /// Web service asynchronous receive of HTTP Accept acknowledgment for one-way asynchronous send operation 'send_ProbeMatches', returns SOAP_OK or error code
        virtual int recv_ProbeMatches_empty_response() { return soap_recv_empty_response(this->soap); }
        //
        /// Web service one-way synchronous send operation 'Resolve' to the default endpoint with the default SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int Resolve(struct wsdd__ResolveType *wsdd__Resolve) { return this->Resolve(NULL, NULL, wsdd__Resolve); }
        /// Web service one-way synchronous send operation 'Resolve' to the specified endpoint and SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int Resolve(const char *soap_endpoint_url, const char *soap_action, struct wsdd__ResolveType *wsdd__Resolve) { return (this->send_Resolve(soap_endpoint_url, soap_action, wsdd__Resolve) || soap_recv_empty_response(this->soap)) ? this->soap->error : SOAP_OK; }
        /// Web service one-way asynchronous send operation 'send_Resolve' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int send_Resolve(struct wsdd__ResolveType *wsdd__Resolve) { return this->send_Resolve(NULL, NULL, wsdd__Resolve); }
        /// Web service one-way asynchronous send operation 'send_Resolve' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_Resolve(const char *soap_endpoint_url, const char *soap_action, struct wsdd__ResolveType *wsdd__Resolve);
        /// Web service one-way asynchronous receive operation 'recv_Resolve', returns SOAP_OK or error code;
        virtual int recv_Resolve(struct __wsdd__Resolve&);
        /// Web service asynchronous receive of HTTP Accept acknowledgment for one-way asynchronous send operation 'send_Resolve', returns SOAP_OK or error code
        virtual int recv_Resolve_empty_response() { return soap_recv_empty_response(this->soap); }
        //
        /// Web service one-way synchronous send operation 'ResolveMatches' to the default endpoint with the default SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int ResolveMatches(struct wsdd__ResolveMatchesType *wsdd__ResolveMatches) { return this->ResolveMatches(NULL, NULL, wsdd__ResolveMatches); }
        /// Web service one-way synchronous send operation 'ResolveMatches' to the specified endpoint and SOAP Action header then wait for HTTP OK/Accept response, returns SOAP_OK or error code
        virtual int ResolveMatches(const char *soap_endpoint_url, const char *soap_action, struct wsdd__ResolveMatchesType *wsdd__ResolveMatches) { return (this->send_ResolveMatches(soap_endpoint_url, soap_action, wsdd__ResolveMatches) || soap_recv_empty_response(this->soap)) ? this->soap->error : SOAP_OK; }
        /// Web service one-way asynchronous send operation 'send_ResolveMatches' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int send_ResolveMatches(struct wsdd__ResolveMatchesType *wsdd__ResolveMatches) { return this->send_ResolveMatches(NULL, NULL, wsdd__ResolveMatches); }
        /// Web service one-way asynchronous send operation 'send_ResolveMatches' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_ResolveMatches(const char *soap_endpoint_url, const char *soap_action, struct wsdd__ResolveMatchesType *wsdd__ResolveMatches);
        /// Web service one-way asynchronous receive operation 'recv_ResolveMatches', returns SOAP_OK or error code;
        virtual int recv_ResolveMatches(struct __wsdd__ResolveMatches&);
        /// Web service asynchronous receive of HTTP Accept acknowledgment for one-way asynchronous send operation 'send_ResolveMatches', returns SOAP_OK or error code
        virtual int recv_ResolveMatches_empty_response() { return soap_recv_empty_response(this->soap); }
    };
#endif
