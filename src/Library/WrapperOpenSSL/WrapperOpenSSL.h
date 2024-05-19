#ifdef OPENSSL_ENABLED
   #include "WrapperOpenSSL_Def.h"
   #include "WrapperOpenSSL_STD.h"

   #include "Crepto_AES.h"
   #include "Crepto_RSA.h"
   #include "WrapperOpenSSL_Utility.h"

   #ifdef _MSC_VER
      #pragma comment(lib, "Crypt32.lib")
   #endif

#endif
