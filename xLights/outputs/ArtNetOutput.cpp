#include "ArtNetOutput.h"

#include <wx/xml/xml.h>
#include <log4cpp/Category.hh>
#ifndef EXCLUDENETWORKUI
#include "ArtNetDialog.h"
#endif
#include "OutputManager.h"
#include "../UtilFunctions.h"

#pragma region Static Variables
int ArtNetOutput::__ip1 = -1;
int ArtNetOutput::__ip2 = -1;
int ArtNetOutput::__ip3 = -1;
bool ArtNetOutput::__initialised = false;
#pragma endregion Static Variables

#pragma region Constructors and Destructors
ArtNetOutput::ArtNetOutput(wxXmlNode* node) : IPOutput(node)
{
    _sequenceNum = 0;
    _datagram = nullptr;
    memset(_data, 0, sizeof(_data));
}

ArtNetOutput::ArtNetOutput() : IPOutput()
{
    _channels = 510;
    _sequenceNum = 0;
    _datagram = nullptr;
    memset(_data, 0, sizeof(_data));
}

ArtNetOutput::~ArtNetOutput()
{
    if (_datagram != nullptr) delete _datagram;
}
#pragma endregion Constructors and Destructors

#pragma region Static Functions
int ArtNetOutput::GetArtNetNet(int u)
{
    return (u & 0x7F00) >> 8;
}

int ArtNetOutput::GetArtNetSubnet(int u)
{
    return (u & 0x00F0) >> 4;
}

int ArtNetOutput::GetArtNetUniverse(int u)
{
    return u & 0x000F;
}

int ArtNetOutput::GetArtNetCombinedUniverse(int net, int subnet, int universe)
{
    return ((net & 0x007F) << 8) + ((subnet & 0x000F) << 4) + (universe & 0x000F);
}

std::list<Output*> ArtNetOutput::Discover(OutputManager* outputManager)
{
    static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    std::list<Output*> res;

    wxByte packet[14];
    memset(packet, 0x00, sizeof(packet));

    packet[0] = 'A';
    packet[1] = 'r';
    packet[2] = 't';
    packet[3] = '-';
    packet[4] = 'N';
    packet[5] = 'e';
    packet[6] = 't';
    packet[9] = 0x20;
    packet[11] = 0x0E; // Protocol version Low
    packet[12] = 0x02;
    packet[13] = 0xe0; //Critical messages Only

    auto localIPs = GetLocalIPs();
    for (auto ip : localIPs)
    {
        wxIPV4address sendlocaladdr;
        sendlocaladdr.Hostname(ip);

        wxDatagramSocket* datagram = new wxDatagramSocket(sendlocaladdr, wxSOCKET_NOWAIT | wxSOCKET_BROADCAST);

        if (datagram == nullptr)
        {
            logger_base.error("Error initialising ArtNet discovery datagram.");
        }
        else if (!datagram->IsOk())
        {
            logger_base.error("Error initialising ArtNet discovery datagram ... is network connected? OK : FALSE");
            delete datagram;
            datagram = nullptr;
        }
        else if (datagram->Error() != wxSOCKET_NOERROR)
        {
            logger_base.error("Error creating ArtNet discovery datagram => %d : %s.", datagram->LastError(), (const char*)DecodeIPError(datagram->LastError()).c_str());
            delete datagram;
            datagram = nullptr;
        }
        else
        {
            logger_base.info("ArtNet discovery datagram opened successfully.");
        }

        // multicast - universe number must be in lower 2 bytes
        wxIPV4address remoteaddr;
        wxString ipaddrWithUniv = "255.255.255.255";
        remoteaddr.Hostname(ipaddrWithUniv);
        remoteaddr.Service(ARTNET_PORT);

        // bail if we dont have a datagram to use
        if (datagram != nullptr)
        {
            logger_base.info("ArtNet sending discovery packet.");
            datagram->SendTo(remoteaddr, packet, sizeof(packet));
            if (datagram->Error() != wxSOCKET_NOERROR)
            {
                logger_base.error("Error sending ArtNet discovery datagram => %d : %s.", datagram->LastError(), (const char*)DecodeIPError(datagram->LastError()).c_str());
            }
            else
            {
                logger_base.info("ArtNet sent discovery packet. Sleeping for 2 seconds.");

                // give the controllers 2 seconds to respond
                wxMilliSleep(2000);

                unsigned char buffer[2048];

                int lastread = 1;

                while (lastread > 0)
                {
                    wxStopWatch sw;
                    logger_base.debug("Trying to read ArtNet discovery packet.");
                    memset(buffer, 0x00, sizeof(buffer));
                    datagram->Read(&buffer[0], sizeof(buffer));
                    lastread = datagram->LastReadCount();

                    if (lastread > 0)
                    {
                        logger_base.debug(" Read done. %d bytes %ldms", lastread, sw.Time());

                        if (buffer[0] == 'A' && buffer[1] == 'r' && buffer[2] == 't' && buffer[3] == '-' && buffer[9] == 0x21)
                        {
                            logger_base.debug(" Valid response.");
                            long channels = 512;
                            ArtNetOutput* output = new ArtNetOutput();
                            output->SetDescription(std::string((char*)& buffer[26]));
                            output->SetChannels(channels);
                            auto ip = wxString::Format("%d.%d.%d.%d", (int)buffer[10], (int)buffer[11], (int)buffer[12], (int)buffer[13]);
                            output->SetIP(ip.ToStdString());

                            // now search for it in outputManager
                            auto outputs = outputManager->GetOutputs();
                            for (auto it = outputs.begin(); it != outputs.end(); ++it)
                            {
                                if ((*it)->GetIP() == output->GetIP())
                                {
                                    // we already know about this controller
                                    logger_base.info("ArtNet Discovery we already know about this controller %s.", (const char*)output->GetIP().c_str());
                                    delete output;
                                    output = nullptr;
                                    break;
                                }
                            }

                            if (output != nullptr)
                            {
                                logger_base.info("ArtNet Discovery adding controller %s.", (const char*)output->GetIP().c_str());
                                res.push_back(output);
                            }
                        }
                        else
                        {
                            // non discovery response packet
                            logger_base.info("ArtNet Discovery strange packet received.");
                        }
                    }
                }
                logger_base.info("ArtNet Discovery Done looking for response.");
            }
            datagram->Close();
            delete datagram;
        }
    }

    logger_base.info("ArtNet Discovery Finished.");

    return res;
}

void ArtNetOutput::SendSync()
{
    static uint8_t syncdata[ARTNET_SYNCPACKET_LEN];
    static wxIPV4address syncremoteAddr;
    static wxDatagramSocket *syncdatagram = nullptr;

    if (!__initialised)
    {
        log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
        logger_base.debug("Initialising artNet Sync.");

        __initialised = true;
        memset(syncdata, 0x00, sizeof(syncdata));
        syncdata[0] = 'A';   // ARTNET flag
        syncdata[1] = 'r';
        syncdata[2] = 't';
        syncdata[3] = '-';
        syncdata[4] = 'N';
        syncdata[5] = 'e';
        syncdata[6] = 't';
        syncdata[9] = 0x52;
        syncdata[11] = 0x0E; // Protocol version Low

        wxIPV4address localaddr;
        if (IPOutput::__localIP == "")
        {
            localaddr.AnyAddress();
        }
        else
        {
            localaddr.Hostname(IPOutput::__localIP);
        }

        if (syncdatagram != nullptr)
        {
            delete syncdatagram;
        }

        syncdatagram = new wxDatagramSocket(localaddr, wxSOCKET_NOWAIT);
        if (syncdatagram == nullptr)
        {
            logger_base.error("Error initialising Artnet sync datagram. %s", (const char *)localaddr.IPAddress().c_str());
            return;
        } else if (!syncdatagram->IsOk())
        {
            logger_base.error("Error initialising Artnet sync datagram ... is network connected? %s OK : FALSE", (const char *)localaddr.IPAddress().c_str());
            delete syncdatagram;
            syncdatagram = nullptr;
            return;
        }
        else if (syncdatagram->Error())
        {
            logger_base.error("Error creating Artnet sync datagram => %d : %s. %s", syncdatagram->LastError(), (const char *)DecodeIPError(syncdatagram->LastError()).c_str(), (const char *)localaddr.IPAddress().c_str());
            delete syncdatagram;
            syncdatagram = nullptr;
            return;
        }

        // broadcast ... this is not really in line with the spec
        // I should use the net mask but i cant find a good way to do that
        //syncremoteAddr.BroadcastAddress();
        wxString ipaddrWithUniv = wxString::Format("%d.%d.%d.%d", __ip1, __ip2, __ip3, 255);
        logger_base.debug("artNet Sync broadcasting to %s.", (const char *)ipaddrWithUniv.c_str());
        syncremoteAddr.Hostname(ipaddrWithUniv);
        syncremoteAddr.Service(ARTNET_PORT);
    }

    if (syncdatagram != nullptr)
    {
        syncdatagram->SendTo(syncremoteAddr, syncdata, ARTNET_SYNCPACKET_LEN);
    }
}
#pragma endregion Static Functions

#pragma region Start and Stop
void ArtNetOutput::OpenDatagram()
{
    log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));

    if (_datagram != nullptr) return;

    wxIPV4address localaddr;
    if (IPOutput::__localIP == "")
    {
        localaddr.AnyAddress();
    }
    else
    {
        localaddr.Hostname(IPOutput::__localIP);
    }

    _datagram = new wxDatagramSocket(localaddr, wxSOCKET_NOWAIT);
    if (_datagram == nullptr)
    {
        logger_base.error("Error initialising Artnet datagram for %s %d:%d:%d. %s", (const char *)_ip.c_str(), GetArtNetNet(), GetArtNetSubnet(), GetArtNetUniverse(), (const char *)localaddr.IPAddress().c_str());
        _ok = false;
    }
    else if (!_datagram->IsOk())
    {
        logger_base.error("Error initialising Artnet datagram for %s %d:%d:%d. %s OK : FALSE", (const char *)_ip.c_str(), GetArtNetNet(), GetArtNetSubnet(), GetArtNetUniverse(), (const char *)localaddr.IPAddress().c_str());
        delete _datagram;
        _datagram = nullptr;
        _ok = false;
    }
    else if (_datagram->Error())
    {
        logger_base.error("Error creating Artnet datagram => %d : %s. %s", _datagram->LastError(), (const char *)DecodeIPError(_datagram->LastError()).c_str(), (const char *)localaddr.IPAddress().c_str());
        delete _datagram;
        _datagram = nullptr;
        _ok = false;
    }
}

bool ArtNetOutput::Open()
{
    log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));

    if (!_enabled) return true;

    _ok = IPOutput::Open();

    memset(_data, 0x00, sizeof(_data));

    _sequenceNum = 0;

    _data[0] = 'A';   // ID[8]
    _data[1] = 'r';
    _data[2] = 't';
    _data[3] = '-';
    _data[4] = 'N';
    _data[5] = 'e';
    _data[6] = 't';
    _data[9] = 0x50;
    _data[11] = 0x0E; // Protocol version Low
    _data[14] = (_universe & 0xFF);
    _data[15] = ((_universe & 0xFF00) >> 8);
    _data[16] = 0x02; // we are going to send all 512 bytes

    OpenDatagram();

    _remoteAddr.Hostname(_ip.c_str());
    _remoteAddr.Service(ARTNET_PORT);

    // work out our broascast address
    wxArrayString ipc = wxSplit(_ip.c_str(), '.');
    if (__ip1 == -1)
    {
        __ip1 = wxAtoi(ipc[0]);
        __ip2 = wxAtoi(ipc[1]);
        __ip3 = wxAtoi(ipc[2]);
    }
    else if (wxAtoi(ipc[0]) != __ip1)
    {
        __ip1 = 255;
        __ip2 = 255;
        __ip3 = 255;
    }
    else
    {
        if (wxAtoi(ipc[1]) != __ip2)
        {
            __ip2 = 255;
            __ip3 = 255;
        }
        else
        {
            if (wxAtoi(ipc[2]) != __ip3)
            {
                __ip3 = 255;
            }
        }
    }

    __initialised = false;
    logger_base.debug("Artnet broadcast address %d.%d.%d.255", __ip1, __ip2, __ip3);

    _data[16] = (uint8_t)(_channels >> 8);  // Property value count (high)
    _data[17] = (uint8_t)(_channels & 0xff);  // Property value count (low)

    return _ok;
}

void ArtNetOutput::Close()
{
    if (_datagram != nullptr)
    {
        delete _datagram;
        _datagram = nullptr;
    }
}
#pragma endregion Start and Stop

#pragma region Frame Handling
void ArtNetOutput::StartFrame(long msec)
{
    log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));

    if (!_enabled) return;

    if (_datagram == nullptr && OutputManager::IsRetryOpen())
    {
        OpenDatagram();
        if (_ok)
        {
            logger_base.debug("ArtNetOutput: Open retry successful");
        }
    }

    _timer_msec = msec;
}

void ArtNetOutput::EndFrame(int suppressFrames)
{
    if (!_enabled || _suspend || _datagram == nullptr) return;

    if (_changed || NeedToOutput(suppressFrames))
    {
        _data[12] = _sequenceNum;
        _datagram->SendTo(_remoteAddr, _data, ARTNET_PACKET_LEN - (512 - _channels));
        _sequenceNum = _sequenceNum == 255 ? 0 : _sequenceNum + 1;
        FrameOutput();
        _changed = false;
    }
    else
    {
        SkipFrame();
    }
}
#pragma endregion Frame Handling

#pragma region Data Setting
void ArtNetOutput::SetOneChannel(long channel, unsigned char data)
{
    wxASSERT(channel < _channels);

    if (_data[channel + ARTNET_PACKET_HEADERLEN] != data) {
        _data[channel + ARTNET_PACKET_HEADERLEN] = data;
        _changed = true;
    }
}

void ArtNetOutput::SetManyChannels(long channel, unsigned char data[], long size)
{
    wxASSERT(channel + size <= _channels);

#ifdef _MSC_VER
    long chs = min(size, _channels - channel);
#else
    long chs = std::min(size, _channels - channel);
#endif

    if (memcmp(&_data[channel + ARTNET_PACKET_HEADERLEN], data, chs) == 0)
    {
        // nothing has changed
    }
    else
    {
        memcpy(&_data[channel + ARTNET_PACKET_HEADERLEN], data, chs);
        _changed = true;
    }
}

void ArtNetOutput::AllOff()
{
    memset(&_data[ARTNET_PACKET_HEADERLEN], 0x00, _channels);
    _changed = true;
}
#pragma endregion Data Setting

#pragma region Getters and Setters
std::string ArtNetOutput::GetUniverseString() const
{
    return wxString::Format(wxT("%i:%i:%i or %d"), GetArtNetNet(), GetArtNetSubnet(), GetArtNetUniverse(), GetUniverse()).ToStdString();
}

std::string ArtNetOutput::GetLongDescription() const
{
    std::string res = "";

    if (!_enabled) res += "INACTIVE ";
    res += "ArtNet " + _ip + " {" + GetUniverseString() + "} ";
    res += "[1-" + std::string(wxString::Format(wxT("%i"), (long)_channels)) + "] ";
    res += "(" + std::string(wxString::Format(wxT("%i"), (long)GetStartChannel())) + "-" + std::string(wxString::Format(wxT("%i"), (long)GetEndChannel())) + ") ";
    res += _description;

    return res;
}

std::string ArtNetOutput::GetChannelMapping(long ch) const
{
    std::string res = "Channel " + std::string(wxString::Format(wxT("%i"), ch)) + " maps to ...\n";

    long channeloffset = ch - GetStartChannel() + 1;

    res += "Type: ArtNet\n";
    res += "IP: " + _ip + "\n";
    res += "Net: " + wxString::Format(wxT("%i"), GetArtNetNet()).ToStdString() + "\n";
    res += "Subnet: " + wxString::Format(wxT("%i"), GetArtNetSubnet()).ToStdString() + "\n";
    res += "Universe: " + wxString::Format(wxT("%i"), GetArtNetUniverse()).ToStdString() + "\n";
    res += "Channel: " + std::string(wxString::Format(wxT("%i"), channeloffset)) + "\n";

    if (!_enabled) res += " INACTIVE";

    return res;
}
#pragma endregion Getters and Setters

#pragma region UI
#ifndef EXCLUDENETWORKUI
Output* ArtNetOutput::Configure(wxWindow* parent, OutputManager* outputManager, ModelManager *modelManager)
{
    ArtNetDialog dlg(parent, this, outputManager);

    int res = dlg.ShowModal();

    if (res == wxID_CANCEL)
    {
        return nullptr;
    }

    return this;
}
#endif
#pragma endregion UI
