/**
 * @file NDPluginChannelMask.cpp
 * @author Ben Bradnick (ben@quantumdetectors.com)
 * @brief Plugin for masking (zeroing) out data from specific Xspress channels
 * @date 2023-12-14
 */

#include "xspressChannelMaskPlugin.h"

#include <epicsExport.h>

static const char *driverName="XspressChannelMaskPlugin";

/** Constructor for XspressChannelMaskPlugin
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] queueSize The number of NDArrays that the input queue for this plugin can hold when
  *      NDPluginDriverBlockingCallbacks=0.  Larger queues can decrease the number of dropped arrays,
  *      at the expense of more NDArray buffers being allocated from the underlying driver's NDArrayPool.
  * \param[in] blockingCallbacks Initial setting for the NDPluginDriverBlockingCallbacks flag.
  *      0=callbacks are queued and executed by the callback thread; 1 callbacks execute in the thread
  *      of the driver doing the callbacks.
  * \param[in] NDArrayPort Name of asyn port driver for initial source of NDArray callbacks.
  * \param[in] NDArrayAddr asyn port driver address for initial source of NDArray callbacks.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is
  *      allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is
  *      allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] maxThreads The maximum number of threads this driver is allowed to use. If 0 then 1 will be used.
  */
XspressChannelMaskPlugin::XspressChannelMaskPlugin(
    const char *portName, int queueSize, int blockingCallbacks,
    const char *NDArrayPort, int NDArrayAddr,
    int maxBuffers, size_t maxMemory,
    int priority, int stackSize, int maxThreads)
    /* Invoke the base class constructor */
    : NDPluginDriver(portName, queueSize, blockingCallbacks,
        NDArrayPort, NDArrayAddr, maxOverlays, maxBuffers, maxMemory,
        asynGenericPointerMask,
        asynGenericPointerMask,
        ASYN_MULTIDEVICE, 1, priority, stackSize, maxThreads
    )
{
    // Create the asyn parameters
    createParam(NDPluginXspressMaskUseString, asynParamInt32, &NDPluginUseMask);
}


/** Configuration command */
extern "C" int XspressChannelMaskConfigure(const char *portName, int queueSize, int blockingCallbacks,
                 const char *NDArrayPort, int NDArrayAddr, 1,
                 int maxBuffers, size_t maxMemory,
                 int priority, int stackSize, int maxThreads)
{
    XspressChannelMaskPlugin *pPlugin = new XspressChannelMaskPlugin(
        portName,
        queueSize,
        blockingCallbacks,
        NDArrayPort,
        NDArrayAddr,
        maxBuffers,
        maxMemory,
        priority,
        stackSize,
        maxThreads
    );

    return pPlugin->start();
}

/* EPICS iocsh shell commands */
static const iocshArg initArg0 = { "portName", iocshArgString };
static const iocshArg initArg1 = { "frame queue size", iocshArgInt };
static const iocshArg initArg2 = { "blocking callbacks", iocshArgInt };
static const iocshArg initArg3 = { "NDArrayPort", iocshArgString };
static const iocshArg initArg4 = { "NDArrayAddr", iocshArgInt };
static const iocshArg initArg5 = { "maxBuffers", iocshArgInt };
static const iocshArg initArg6 = { "maxMemory", iocshArgInt };
static const iocshArg initArg7 = { "priority", iocshArgInt };
static const iocshArg initArg8 = { "stackSize", iocshArgInt };
static const iocshArg initArg9 = { "maxThreads", iocshArgInt };
static const iocshArg * const initArgs[] = {
    &initArg0,
    &initArg1,
    &initArg2,
    &initArg3,
    &initArg4,
    &initArg5,
    &initArg6,
    &initArg7,
    &initArg8,
    &initArg9
};

static const iocshFuncDef initFuncDef = {"NDPluginXspressChannel", 10, initArgs};

static void initCallFunc(const iocshArgBuf *args)
{
    XspressChannelMaskConfigure(
        args[0].sval,
        args[1].ival,
        args[2].ival,
        args[3].sval,
        args[4].ival,
        args[5].ival,
        args[6].ival,
        args[7].ival,
        args[8].ival,
        args[9].ival
    );
}

extern "C" void XspressChannelMaskRegister(void)
{
    iocshRegister(&initFuncDef, initCallFunc);
}

extern "C" {
    epicsExportRegistrar(XspressChannelMaskRegister);
}

