# Introduction
**pml_log** is a simple thread safe logging library for C++

# Dependencies
The library uses [moodycamel::concurrentqueue](https://github.com/cameron314/concurrentqueue)

If you build the library using **cmake** then this repo will be cloned automatically for you.

# Compiling
The library uses **cmake** to handle fetching the required dependencies and configuring the build.
There are the following custom **cmake** variables used to define the location to clone the dependenies to

- DIR_BASE (default: the root directory of **log**/external) the base location for the external libraries
- DIR_QUEUE (default: DIR_BASE/concurrentqueue) the location of concurrentqueue

# How it works
The library consists of two public classes `Stream` and `Output` and an internal class `Manager`
The `Stream` class contains a `std::stringstream` and a log level variable. When a `Stream` is flushed, either by the program passing `std::endl;` or in the `Stream` destructor, 
the contents of the stream are added to a queue in the `Manager` class.
The `Manager` class runs is a separate thread and has a simple loop to handle the log messages added to the queue. It send each message to all the defined `Output` objects.
The `Output` class defines what should be done with the log message. The base class checks whether the level of the log message exceeds a configured minimum level and if so outputs the message to `std::cout`.
There is also a `File` class derived from the base `Output` class that saves the message to a file.

It is possible for the developer to derive their own Output classes to handle log messages in other ways.

# Usage

At the start of your program you should assign one or more output objects to the `Manager`
```C++
// Output log messages to console
auto nOutputId = pml::log::Stream::AddOutput(std::make_unique<pml::log::Output>());
//Output log messages to files
auto nFileId = pml::log::Stream::AddOutput(std::make_unique<pml::log::File>("/var/log/myprog));
```

To create a log message you can simply call the helper functions
```C++
pml::log::info("myprog") << "This is an info message"
pml::log::debug("myprog") << "And this is a debug message"
```

You can also create a Stream object and keep adding to the log message to it
```C++
auto stream = pml::log::Stream(pml::log::Level::kWarning, "myprog");
stream << "This is the start of my very long warning log \n";
...
stream << "and this is the end of my log message";
```
To decide what log messages get displayed on the console, saved to file etc you can set the minimum **Level** of log message to display.
The levels are
- pml::log::Level::kTrace
- pml::log::Level::kDebug
- pml::log::Level::kInfo
- pml::log::Level::kWarning
- pml::log::Level::kError
- pml::log::Level::kCritical
```C++
//Set minimum output level for all outputs to be kInfo
pml:log::Stream::SetOutputLevel(pml::log::Level::kInfo);

//set the minimum output level for the console output to be kTrace
pml::log::Stream::SetOutputLevel(nOutputId, pml::log::Level::kTrace);
```

Before your application exits you must stop the `Manager` thread 
```C++
// stop logging thread cleanly
pml::log::Stream::Stop();
return 0;
```



