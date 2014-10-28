//#include <cuda_runtime_api.h>
#ifdef BONSAI_CATALYST_STDLIB
 #include <boost/lexical_cast.hpp>
 #define bonsaistd boost
 #define to_string boost::lexical_cast<std::string>
#else
 #define to_string std::to_string
#endif

#include "SharedMemory.h"
#include "BonsaiSharedData.h"

int main(int argc, char * argv[])
{
  using ShmQHeader = SharedMemoryServer<BonsaiSharedQuickHeader>;
  using ShmQData   = SharedMemoryServer<BonsaiSharedQuickData>;
  using ShmSHeader = SharedMemoryServer<BonsaiSharedSnapHeader>;
  using ShmSData   = SharedMemoryServer<BonsaiSharedSnapData>;


  const int n = argc > 1 ? atoi(argv[1]) : 1;
  fprintf(stderr, "cleaning for n= %d\n", n);
  for (int i = 0; i < n; i++)
  {
#if 0
    fprintf(stderr, "clear: %s \n", ShmQHeader::type::sharedFile(i));
    fprintf(stderr, "clear: %s \n", ShmQData  ::type::sharedFile(i));
    fprintf(stderr, "clear: %s \n", ShmSHeader::type::sharedFile(i));
    fprintf(stderr, "clear: %s \n", ShmSData  ::type::sharedFile(i));
#endif
    ShmQHeader shmQHeader(ShmQHeader::type::sharedFile(i), 1);
    ShmQData   shmQData  (ShmQData  ::type::sharedFile(i), 1);

    ShmSHeader shmSHeader(ShmSHeader::type::sharedFile(i), 1);
    ShmSData   shmSData  (ShmSData  ::type::sharedFile(i), 1);
  }

  return 0;
}

