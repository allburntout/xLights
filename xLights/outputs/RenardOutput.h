#ifndef RENARDOUTPUT_H
#define RENARDOUTPUT_H

#include "SerialOutput.h"

// Should be called with: 0 <= chindex <= 1015 (max channels=127*8)

#pragma region Renard Constants
#define RENARD_MAX_CHANNELS 1015
#pragma endregion Renard Constants

class RenardOutput : public SerialOutput
{
    #pragma region Member Variables
    int _datalen;
    std::vector<uint8_t> _data;
    #pragma endregion Member Variables

public:

    #pragma region Constructors and Destructors
    RenardOutput(SerialOutput* output);
    RenardOutput(wxXmlNode* node);
    RenardOutput();
    virtual ~RenardOutput() override {};
    #pragma endregion Constructors and Destructors

    #pragma region Getters and Setters
    virtual std::string GetType() const override { return OUTPUT_RENARD; }
    virtual int GetMaxChannels() const override { return RENARD_MAX_CHANNELS; }
    virtual bool IsValidChannelCount(long channelCount) const override { return channelCount > 0 && channelCount <= RENARD_MAX_CHANNELS; }
    virtual std::string GetSetupHelp() const override;
    #pragma endregion Getters and Setters

    #pragma region Start and Stop
    virtual bool Open() override;
    #pragma endregion Start and Stop

    #pragma region Frame Handling
    virtual void EndFrame(int suppressFrames) override;
    #pragma endregion Frame Handling

    #pragma region Data Setting
    virtual void SetOneChannel(long channel, unsigned char data) override;
    virtual void AllOff() override;
    #pragma endregion Data Setting
};

 #endif