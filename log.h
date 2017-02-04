#ifndef _LOG_H_
#define _LOG_H_

#define LOG_ERR(msg, ...)			\
    do {                                        \
	fprintf(stderr,				\
		"%s:%d: ERR: "msg,		\
		__FILE__,			\
		__LINE__,			\
		##__VA_ARGS__);			\
    } while(0)

#define LOG_INFO(msg, ...)			\
      do {					\
	  fprintf(stderr,			\
		  "%s:%d: INFO: "msg,		\
		  __FILE__,			\
		  __LINE__,			\
		  ##__VA_ARGS__);		\
      } while(0)


#endif
