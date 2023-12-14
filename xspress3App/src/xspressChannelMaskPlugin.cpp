/**
 * @file NDPluginChannelMask.cpp
 * @author Ben Bradnick (ben@quantumdetectors.com)
 * @brief Plugin for masking (zeroing) out data from specific Xspress channels
 * @date 2023-12-14
 */

#include "xspressChannelMaskPlugin.h"

#include <iocsh.h>

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
        NDArrayPort, NDArrayAddr, 1, maxBuffers, maxMemory,
        asynGenericPointerMask,
        asynGenericPointerMask,
        ASYN_MULTIDEVICE, 1, priority, stackSize, maxThreads
    )
{
    // Create the asyn parameters
    createParam(XspressChannelMaskPluginUseString, asynParamInt32, &NDPluginUseMask);
}


/**
 * @brief Callback function that is called by the NDArray driver with new NDArray data.
 * 
 * @param pArray New NDArray from the NDArray driver
 */
void XspressChannelMaskPlugin::processCallbacks(NDArray *pArray)
{
    /* Call the base class method */
    NDPluginDriver::beginProcessCallbacks(pArray);

    // TODO: copy the array

    // TODO: implement the callback to apply the mask

    // TODO: call endProcessCallbacks properly
    /** Method that is normally called at the end of the processCallbacks())
     * method in derived classes.
     * \param[in] pArray  The NDArray from the callback.
     * \param[in] copyArray This flag should be true if pArray is the original array passed to processCallbacks().
     *            It must be false if the derived class if pArray is a new NDArray that processCallbacks() created
     * \param[in] readAttributes This flag must be true if the derived class has not yet called readAttributes() for pArray.
     *
     * This method does NDArray callbacks to downstream plugins if NDArrayCallbacks is true and SortMode is Unsorted.
     * If SortMode is sorted it inserts the NDArray into the std::multilist for callbacks in SortThread().
     * It keeps track of DisorderedArrays and DroppedOutputArrays.
     * It caches the most recent NDArray in pArrays[0]. */
    NDPluginDriver::endProcessCallbacks(pArray, true, true);
}


/**
 * @brief Called when asyn clients call pasynInt32->write().
 * 
 * Overidden from the base class to set plugin parameters.
 * 
 * @param pasynUser pasynUser structure that encodes the reason and address.
 * @param value Value to write
 * @return asynStatus Whether write was successful or not
 */
asynStatus XspressChannelMaskPlugin::writeInt32(asynUser *pasynUser, epicsInt32 value) {
    asynStatus status = asynSuccess;
    int param = pasynUser->reason;

    // Call the base class
    status = NDPluginDriver::writeInt32(pasynUser, value);

    return (asynStatus) status;
}


/**
 * @brief Apply the channel mask based on the selected channels to mask out
 * 
 * @param pArray NDArray to apply mask to
 */
void XspressChannelMaskPlugin::applyMask(NDArray *pArray)
{
    // TODO: implement function
}


/** Configuration command */
extern "C" int XspressChannelMaskConfigure(
    const char *portName,
    int queueSize,
    int blockingCallbacks,
    const char *NDArrayPort,
    int NDArrayAddr,
    int maxBuffers,
    size_t maxMemory,
    int priority,
    int stackSize,
    int maxThreads)
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

