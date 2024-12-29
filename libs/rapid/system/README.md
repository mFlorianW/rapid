# System Module
The purpose of this modules is to provide some operating system independent features like timer, synchronization etc. 
All the elements in the module are developed with a event driven approach, that means that most of the implementations will have signals to notifier about changes that then can be handled by the caller.

**Restrictions**
The implementations are not thread safe but the classes are reentrant. That means all classes shouldn't be used in different threads.
The Timer and FutureWatcher implementation are only usable from the main loop.
These implementation uses a internal registry for the event handling. 
Thre registry there can't be used from different threads because then the signals of the these classes would be emitted from different threads.

## System Task
The system task implements the event handling for most of the elements in that module. The system task must be called
periodically in the application main loop. For a good responsiveness the system task should be called every 5ms. 

## Timer
A timer can be used to periodically a function or just get notified when the timeout occur.

## FutureWatcher
The FutureWatcher can observe a std::future and emits a finish signal when the future has a value or the execution of thread is completed.

