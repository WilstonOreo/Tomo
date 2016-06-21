#pragma once
#include <iostream>

#ifdef DEBUG
#define TOMO_MSG_(stream,type,msg)\
  stream << type << ":" << __FILE__ << ":" << __LINE__ << ":" << __func__ << ": " << msg
#define TOMO_DBG_(stream,msg) TOMO_MSG_(stream,"DBG",msg)
#define TOMO_DBG(msg) TOMO_DBG_(std::cout,msg) << std::endl;
#else
#define TOMO_MSG_(stream,type,msg) stream << type << ": " << msg
#define TOMO_DBG_(stream,msg)
#define TOMO_DBG(msg)
#endif

#define TOMO_MSG(type,msg) TOMO_MSG_(std::cout,type,msg) << std::endl;
#define TOMO_WRN_(stream,msg) TOMO_MSG_(stream,"WRN",msg)
#define TOMO_WRN(msg) TOMO_WRN_(std::cout,msg) << std::endl;
#define TOMO_ERR_(stream,msg) TOMO_MSG_(stream,"ERR",msg)
#define TOMO_ERR(msg) TOMO_ERR_(std::cout,msg) << std::endl;
