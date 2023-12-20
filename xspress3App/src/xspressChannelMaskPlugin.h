/**
 * @file NDPluginChannelMask.h
 * @author Ben Bradnick (ben@quantumdetectors.com)
 * @brief Plugin for masking (zeroing) out data from specific Xspress channels
 * @date 2023-12-14
 */

#ifndef XspressChannelMaskPlugin_H
#define XspressChannelMaskPlugin_H

#include "NDPluginDriver.h"


#define XspressChannelMaskPluginUseString  "USE"         // (asynInt32,   r/w) String for enabling mask

/**
 * @brief AreaDetector plugin to mask out specific Xspress channels
 * 
 */
class XspressChannelMaskPlugin : public NDPluginDriver {

public:
    XspressChannelMaskPlugin(
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
    );

    ~XspressChannelMaskPlugin() {};

    void processCallbacks(NDArray *pArray) override;
    asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value) override;

protected:
    int NDPluginUseMask; // Whether to use the mask
    std::vector<int> enabledChannels; // Vector of enabled channels

private:
    void applyMask(NDArray *pArray);

};

#endif // XspressChannelMaskPlugin_H
