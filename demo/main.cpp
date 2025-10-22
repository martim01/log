#include <iostream>
#include "log.h"

int main()
{
    // register console output
    pml::log::Stream::AddOutput(std::make_unique<pml::log::Output>());

    pml::log::log(pml::log::Level::kInfo, "demo") << "Info message from demo";
    pml::log::log(pml::log::Level::kDebug, "demo") << "Debug message from demo";
    pml::log::log(pml::log::Level::kError, "demo") << "Error message from demo";

    // give the logger thread a moment to process the queue
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // stop logging thread cleanly
    pml::log::Stream::Stop();

    return 0;
}
