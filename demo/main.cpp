#include <iostream>
#include "log.h"

int main()
{
    // register console output
    pml::log::Stream::AddOutput(std::make_unique<pml::log::Output>());

    pml::log::info("demo") << "Info message from demo";
    pml::log::debug("demo") << "Debug message from demo";
    pml::log::error("demo") << "Error message from demo";

    // stop logging thread cleanly
    pml::log::Stream::Stop();

    return 0;
}
