#include <iostream>
#include "../src/logging.h"


int main(int argc, char* argv[]) {
  //auto logging = logging::Logging::logger();
  auto logger = logging::getLogger("CPP Logger");
  logger->debug("Test debug C++ from C++.");
  logger->info("Test info C++ from C++.");
  logger->warning("Test warning C++ from C++.");
  logger->error("Test error C++ from C++.");
  logger->critical("Test critical C++ from C++.");
}
