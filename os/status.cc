// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "status.hpp"

Status SocketReadFailed =
    new StatusClass (1, "[SocketReadFailed]: failed to read from socket");
Status SocketWriteFailed =
    new StatusClass (1, "[SocketWriteFailed]: failed to write to socket");
Status SocketShutdownFailed =
    new StatusClass (1, "[SocketShutdownFailed]: failed to shutdown socket");
Status FileReadFailed =
    new StatusClass (1, "[FileReadFailed]: failed to read from file");
Status FileWriteFailed =
    new StatusClass (1, "[FileWriteFailed]: failed to write to file");
Status PipeReadFailed =
    new StatusClass (1, "[PipeReadFailed]: failed to read from pipe");
Status PipeWriteFailed =
    new StatusClass (1, "[PipeWriteFailed]: failed to write to pipe");
Status PipeRCloseFailed =
    new StatusClass (1, "[PipeRCloseFailed]: failed to close read fd from pipe");
Status PipeWCloseFailed =
    new StatusClass (1, "[PipeWCloseFailed]: failed to close write fd from pipe");
Status ArgInvalidFD =
    new StatusClass(1, "[ArgInvalidFD]: arg is an invalid file descriptor");
Status EventLoopMonitorFDFailed =
    new StatusClass(1, "[EventLoopMonitorFDFailed] event loop "
                    "failed to monitor file descriptor");
Status EventLoopUnmonitorFDFailed =
    new StatusClass(1, "[EventLoopUnmonitorFDFailed] event loop "
                    "failed to monitor file descriptor");
