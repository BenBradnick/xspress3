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
    const char *portName,
    int numChannels,
    int queueSize,
    int blockingCallbacks,
    const char *NDArrayPort,
    int NDArrayAddr,
    int maxBuffers,
    size_t maxMemory,
    int priority,
    int stackSize,
    int maxThreads
    )
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

    for (int i=0; i<numChannels; i++)
    {
        std::string channelString("CH" + std::to_string(i) + "Enable");
        int paramValue;
        createParam(channelString.c_str(), asynParamInt32, &paramValue);
        enabledChannels.push_back(paramValue);
    }

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

    int apply;
    getIntegerParam(NDPluginUseMask, &apply);

    if (apply == 1)
    {
        printf("%s: applying mask\n", driverName);

        this->unlock();

        // Copy the array
        NDArray *pArrayCopy = this->pNDArrayPool->copy(pArray, NULL, true);

        // TODO: apply the channel mask based on selected user options
        

        this->lock();

        // Publish the modified array
        NDPluginDriver::endProcessCallbacks(pArrayCopy, false, true);

    }
    else
    {
        // No mask applied - publish original array
        printf("%s: skipping mask\n", driverName);
        NDPluginDriver::endProcessCallbacks(pArray, true, true);
    }
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

    // Check if one of our parameters
    bool pluginParam = false;
    if (param == NDPluginUseMask)
    {
        pluginParam = true;
        printf("%s: use mask = %d\n", driverName, value);
        status = (asynStatus) setIntegerParam(param, value);
        callParamCallbacks();
    }
    else
    {
        for (unsigned int channel = 0; channel < enabledChannels.size(); channel++)
        {
            if (param == (unsigned int) enabledChannels[channel])
            {
                pluginParam = true;
                printf("%s: channel %d enable: %d\n", driverName, channel, value);
                status = (asynStatus) setIntegerParam(param, value);
                callParamCallbacks();
                break;
            }
        }
    }

    // Call the base driver if we haven't handled it
    if (pluginParam == false)
    {
        status = (asynStatus) NDPluginDriver::writeInt32(pasynUser, value);
    }

    return status;
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


extern "C" {

    /** Configuration command */
    int XspressChannelMaskConfigure(
        const char *portName,
        int numChannels,
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
            numChannels,
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

    /* EPICS iocsh shell register commands */

    static const iocshArg maskConfigArg0 = { "Asyn port name", iocshArgString };
    static const iocshArg maskConfigArg1 = { "Number of channels", iocshArgInt };
    static const iocshArg maskConfigArg2 = { "Max queue size", iocshArgInt };
    static const iocshArg maskConfigArg3 = { "Blocking callbacks", iocshArgInt };
    static const iocshArg maskConfigArg4 = { "NDArrayPort", iocshArgString };
    static const iocshArg maskConfigArg5 = { "NDArrayAddr", iocshArgInt };
    static const iocshArg maskConfigArg6 = { "maxBuffers", iocshArgInt };
    static const iocshArg maskConfigArg7 = { "maxMemory", iocshArgInt };
    static const iocshArg maskConfigArg8 = { "priority", iocshArgInt };
    static const iocshArg maskConfigArg9 = { "stackSize", iocshArgInt };
    static const iocshArg maskConfigArg10 = { "maxThreads", iocshArgInt };
    static const iocshArg * const maskConfigArgs[] = {
        &maskConfigArg0,
        &maskConfigArg1,
        &maskConfigArg2,
        &maskConfigArg3,
        &maskConfigArg4,
        &maskConfigArg5,
        &maskConfigArg6,
        &maskConfigArg7,
        &maskConfigArg8,
        &maskConfigArg9,
        &maskConfigArg10
    };

    static const iocshFuncDef channelMaskDefinition = {
        "XspressChannelMaskConfigure",
        11,
        maskConfigArgs
    };

    static void initCallFunc(const iocshArgBuf *args)
    {
        XspressChannelMaskConfigure(
            args[0].sval,
            args[1].ival,
            args[2].ival,
            args[3].ival,
            args[4].sval,
            args[5].ival,
            args[6].ival,
            args[7].ival,
            args[8].ival,
            args[9].ival,
            args[10].ival
        );
    }

    static void XspressChannelMaskRegister(void)
    {
        iocshRegister(&channelMaskDefinition, initCallFunc);
    }

    epicsExportRegistrar(XspressChannelMaskRegister);

}
