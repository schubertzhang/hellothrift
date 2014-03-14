#
# Interface definition for HelloService
#

# ########################################################################
# *** PLEASE REMEMBER TO EDIT THE VERSION CONSTANT WHEN MAKING CHANGES ***
# ########################################################################


# namespace
namespace java org.apache.thrift.hello.service
namespace php  org.apache.thrift.hello.service

#
# data structures
#
struct HelloStruct {
  1: required bool   a = false,
  2: required byte   b = 0,
  3: required i16    c = 0,
  4: required i32    d = 0,
  5: required i64    e = 0,
  6: required double f = 0.0,
  7: optional string g = "hello",
  8: optional binary h
}

#
# Exceptions
# (note that internal server errors will raise a TApplicationException, courtesy of Thrift)
#
exception InvalidRequestException {
    1: required string why
}


#
# Service
#
service HelloService {
  string helloEcho(1:required string message, 2:required HelloStruct body)
                  throws (1:InvalidRequestException ire),
  oneway void hello(1:required string message, 2:required HelloStruct body)
}


service Hello2Service {
  string hello2Echo(1:required string message),
}
