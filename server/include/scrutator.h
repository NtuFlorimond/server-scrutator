#ifndef SCRUTATOR_MODULE_H
#define SCRUTATOR_MODULE_H

extern "C" {
#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include "apr_strings.h"
#include "http_request.h"
}

extern "C" int scrutator_handler(request_rec* r);
#endif 
