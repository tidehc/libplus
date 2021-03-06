/*
* plusprocess.h
*
* Copyright (c) 2015 Spranto International Pte Ltd. All Rights Reserved.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, you can obtain one at http://mozilla.org/MPL/2.0/
*
* Software distributed under the License is distributed on an "AS IS"
* basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
* the License for the specific language governing rights and limitations
* under the License.
*
* Contributor(s): ______________________________________.
*
* $Id$
*
*/

#pragma once

#include <vector>
#include <map>
#include <queue>

///////////////////////////////////////////////////////////////////////////////////////////
///

class PlusEndpoint;
class PlusMediaManager : public H323_MediaManager
{
public:

    PlusMediaManager();

    enum MediaStream {
        e_audioIn,
        e_audioOut,
        e_videoIn,
        e_videoOut,
        e_contentIn,
        e_contentOut,
        e_localVideoOut,
        e_NoOfMediaStream
    };

    struct Sample : public PBYTEArray
    {
        Sample(void * data, unsigned size, unsigned width=0, unsigned height=0);

        unsigned m_width;
        unsigned m_height;
    };

    struct Queue : public std::queue<Sample>
    {
        Queue();

        Queue(unsigned v1, unsigned v2, const PString & v3, unsigned v4, unsigned v5);

        unsigned m_v1;   // Audio sampleRate        Video Width
        unsigned m_v2;   // Audio bytes/sample      Video Height
        PString  m_v3;   // Audio NA                Video ColourFormat
        unsigned m_v4;   // Audio #Channels         Video NA
        unsigned m_v5;   // Audio SampleTime        Video NA

        PMutex   m_mutex;
        bool     m_isRunning;
        bool     m_shutdown;
    };

    PStringArray SupportedFormats(unsigned dir);

    bool ProcessMediaSamples(unsigned & id, void * data, unsigned & size, unsigned & width, unsigned & height);

    void SetAudioFormat(unsigned sampleRate, unsigned bytesPerSample, unsigned noChannels, unsigned sampleTime);

    virtual PBoolean SetAudioFormat(unsigned id, unsigned sampleRate, unsigned bytesPerSample, unsigned noChannels, unsigned sampleTime);
    virtual void     GetAudioFormat(unsigned id, unsigned & sampleRate, unsigned & bytesPerSample, unsigned & noChannels, unsigned & sampleTime);

    virtual PBoolean SetColourFormat(unsigned id, const PString & colourFormat);
    virtual void     GetColourFormat(unsigned id, PString & colourFormat);

    virtual PBoolean Start(unsigned id);
    virtual PBoolean Stop(unsigned id);
    virtual PBoolean IsRunning(unsigned id);

    virtual void SetVideoFormat(H323Channel::Directions dir, const PString & colourFormat);

    virtual PBoolean GetFrameSize(unsigned id, unsigned & width, unsigned & height);
    virtual PBoolean SetFrameSize(unsigned id, unsigned width, unsigned height);

    virtual bool Write(unsigned id, void * data, unsigned size, unsigned width=0, unsigned height=0);

    virtual bool Read(unsigned id, bool toBlock, void * data, unsigned size);
    virtual bool Read(unsigned id, bool toBlock, void * data, unsigned & size, unsigned & width, unsigned & height);

private:

    map<unsigned,Queue> m_queueMedia;

};



///////////////////////////////////////////////////////////////////////////////////////////
// PlusEndPoint MACROS

#define PlusSetting(name) \
    public: \
    PString get_##name() \
    { PTRACE(6,"EP\tGet setting m_" << #name << " val: " << m_##name); \
        return m_##name; } \
    void set_##name(const PString & value) \
    { PTRACE(6,"EP\tSet setting m_" << #name << " from " << m_##name << " to " << value); \
        m_##name = value; \
        PString dft = "*"; \
        HandleSettingChange(PlusProcess::e_##name, #name, value, dft); \
    } \
    void ini_##name(const PString & value) \
    {   PString tempval = value; \
        HandleSettingChange(PlusProcess::e_##name, #name, "", tempval); \
        PTRACE(6,"EP\tInitial m_" << #name << " to " << tempval); \
        m_##name = tempval; \
    } \
    protected: \
    PString m_##name;

#define PlusMethod(name) \
    public: \
    void Do##name(const PString & p1, const PString & p2, const PString & p3, const PString & p4) 

#define PlusEvent(name) \
    void fire_##name() \
    { m_process.HandleEvent(PlusProcess::e_##name); }

#define PlusEvent1(name) \
    void fire_##name(const std::string & p1) \
    { m_process.HandleEvent(PlusProcess::e_##name,p1); }

#define PlusEvent2(name) \
    void fire_##name(const std::string & p1, const std::string & p2) \
    { m_process.HandleEvent(PlusProcess::e_##name,p1,p2); }

#define PlusEvent3(name) \
    void fire_##name(const std::string & p1, const std::string & p2, const std::string & p3) \
    { m_process.HandleEvent(PlusProcess::e_##name,p1,p2,p3); }

#define PlusEvent4(name) \
    void fire_##name(const std::string & p1, const std::string & p2, const std::string & p3, const std::string & p4) \
    { m_process.HandleEvent(PlusProcess::e_##name,p1,p2,p3,p4); }

#define IsSet(name,val) \
    (m_##name == #val)

#define IsNSet(name,val) \
    (m_##name != #val)

#define PlusSetValue(name,val) \
    m_##name = val; \
    fire_is##name(m_##name);

#define PlusMedia(name) \
    bool in##name(void * data, int size, int width=0, int height=0); \
    virtual bool out##name(void * data, int size, int width=0, int height=0) { return m_process.out##name(data, size, width, height); };


class PlusEndPoint : public H323EndPoint
{
    PCLASSINFO(PlusEndPoint, H323EndPoint);

public:
#ifdef H323_DATASTORE
    PlusEndPoint(PlusProcess & _process, H323DataStore & _data);
#else
    PlusEndPoint(PlusProcess & _process);
#endif

    ~PlusEndPoint();

    // Settings Macros
    PlusSetting(version)
    PlusSetting(tracing)
    PlusSetting(username)
    PlusSetting(password)
    PlusSetting(server)
    PlusSetting(quality)
    PlusSetting(accessability)
    PlusSetting(content)
    PlusSetting(autoanswer)

    PlusSetting(drvvideoplay)
    PlusSetting(drvvideorec)
    PlusSetting(drvaudioplay)
    PlusSetting(drvaudiorec)
    PlusSetting(curdrvvideoplay)
    PlusSetting(curdrvvideorec)
    PlusSetting(curdrvaudioplay)
    PlusSetting(curdrvaudiorec)
    PlusSetting(audiorec)
    PlusSetting(audioplay)
    PlusSetting(videorec)
    PlusSetting(videoplay)

    PlusSetting(call)
    PlusSetting(audiomute)
    PlusSetting(videomute)
    PlusSetting(framewidth)
    PlusSetting(frameheight)
    PlusSetting(devvideoplay)
    PlusSetting(devvideorec)
    PlusSetting(devaudioplay)
    PlusSetting(devaudiorec)
    PlusSetting(callstate)
    PlusSetting(h281support)
    PlusSetting(t140support)
    PlusSetting(h284support)
    PlusSetting(h281call)
    PlusSetting(t140call)
    PlusSetting(h284call)
    PlusSetting(usere164)
    PlusSetting(useruri)
    PlusSetting(showlocalvideo)
    PlusSetting(initialised)
    PlusSetting(language)
    PlusSetting(listenport)

    PlusSetting(videoformats)
    PlusSetting(videoinformat)
    PlusSetting(videooutformat)
    PlusSetting(secondvideo)

    PlusSetting(encryptsignal)
    PlusSetting(encryptmedia)
    PlusSetting(encryptmediahigh)

    // Internal Use Only
    PlusSetting(dhOID)
    PlusSetting(dhPrime)
    PlusSetting(dhGenerator)
    // IMPL: Setting Names here


    // Method macros
    PlusMethod(placecall) { PlaceCall(p1); }
    PlusMethod(hangup) { HangUp(); }
    PlusMethod(answer) { AnswerCall(); }
    PlusMethod(dtmf) { DTMF(p1); }
    PlusMethod(fecc) { FECC(p1, p2, p3); }
    PlusMethod(h281text) {}
    PlusMethod(h281return) {}
    PlusMethod(h284instruction) { SendH284Instruction((int)p1.AsInteger(), (int)p2.AsInteger(), (int)p3.AsInteger()); }

    PlusMethod(start) { Initialise(); }
    PlusMethod(stop) { UnInitialise(); }
    PlusMethod(secondcall) {}
    PlusMethod(dhParameters) 
    { 
#ifdef H323_H235
        GenerateParameters();
#endif
    }
    PlusMethod(videosize) { m_mediaManager.SetFrameSize((unsigned)p1.AsInteger(), (unsigned)p2.AsInteger(), (unsigned)p3.AsInteger()); }
    PlusMethod(audioformat) { m_mediaManager.SetAudioFormat((unsigned)p1.AsInteger(), (unsigned)p2.AsInteger(), (unsigned)p3.AsInteger(), (unsigned)p4.AsInteger()); }
    PlusMethod(userMethod) {}   // Not used but there for GCC warning
    // IMPL: Method Names here

    // Event macros
    PlusEvent2(status);
    //PlusEvent1(notused1);
    PlusEvent1(isinitialised)
    PlusEvent1(callerid)
    PlusEvent1(incomingcall)
    PlusEvent1(incall)
    PlusEvent1(encryption)
    PlusEvent1(ish281call)
    PlusEvent1(ist140call)
    PlusEvent1(ish284call)
    PlusEvent1(isusere164)
    PlusEvent1(isuseruri)
    PlusEvent1(presence)
    PlusEvent(duplicate)
    PlusEvent1(forwardcall)
    PlusEvent1(dhGenerate)
    PlusEvent2(mediastart)
    // IMPL: Event Names here


    // Media Handling
    PlusMedia(audio)
    PlusMedia(video)
    PlusMedia(content)


    // overrides from h323plus
    virtual H323Connection * CreateConnection(unsigned callReference, void * userData);
    virtual void OnConnectionEstablished(H323Connection & connection, const PString & token);
    virtual void OnCallClearing(H323Connection * connection, H323Connection::CallEndReason reason);
    virtual void OnConnectionCleared(H323Connection & connection, const PString & clearedCallToken);
    virtual PBoolean OnIncomingCall(H323Connection & connection, const H323SignalPDU & setupPDU, H323SignalPDU &);
    virtual void OnClosedLogicalChannel(H323Connection & connection, const H323Channel & channel);
    virtual H323Connection::AnswerCallResponse OnAnswerCall(H323Connection &, const PString &, const H323SignalPDU &, H323SignalPDU &);
    virtual void SetVendorIdentifierInfo(H225_VendorIdentifier & info) const;
    virtual PBoolean STUNNatType(int type);
    virtual void OnMediaEncryption(unsigned session, H323Channel::Directions dir, const PString & cipher);
    virtual PBoolean OnGatekeeperAliases(const H225_ArrayOf_AliasAddress & aliases);

#ifdef H323_H460P
    virtual void PresenceInstruction(const PString & locAlias, unsigned type, const PString & subAlias,
                                     const PString & subDisplay, const PString & subAvatar);
#endif

    virtual PBoolean OpenAudioChannel(H323Connection &, PBoolean, unsigned, H323AudioCodec &);
    virtual PBoolean OpenVideoChannel(H323Connection &, PBoolean, H323VideoCodec &);

    // Internal Properties 
    PConfig * GetConfiguration(const PString & device);
    H323Capability::CapabilityFrameSize GetFrameSize();
    int GetVideoBitRate();
    PBoolean GetEmphasisSpeed();
    PBoolean GetContent();

#ifdef H323_H224
    PBoolean IsH224Active(PBoolean isTransmitter, const PString & id);
    void H224RemoteSupportDetected(const PString & id);
#endif

#ifdef H224_H281
    void StartH281Action(H281_Frame::PanDirection panDirection,
                        H281_Frame::TiltDirection tiltDirection,
                        H281_Frame::ZoomDirection zoomDirection,
                        H281_Frame::FocusDirection focusDirection);
    void StopH281Action();
#endif

    PlusMediaManager &  GetMediaManager();

    void InitialiseSettings(bool reset = false);

protected:

    /*  HandleSettingChange
        Handle changes in settings (if required)
        @return@
        success of the request
    */
    void HandleSettingChange(PlusProcess::Setting setting, const PString & name, const PString & value, PString & dftValue);

    /*  FireStatus
        Fire an status event in the users language back to the user
    */
    void FireStatus(
        int status           // status from the EPStatus enumerator
        );

    void Initialise();
    void UnInitialise();

#ifdef H323_TLS   
    PBoolean InitialiseSignallingEncryption();
#endif

#ifdef H323_H235   
    PBoolean InitialiseMediaEncryption();
#ifdef H323_H235_AES256
    void GenerateParameters();
#endif
#endif

#ifdef PTRACING
    void InitialiseDebug();
#endif

    PBoolean InitialiseEndpoint();
    PBoolean RegisterGatekeeper();
    PBoolean InitialiseGateway();
    void PlaceCall(const PString & address);
    void HangUp();
    void AnswerCall();
    void DTMF(const PString & p1);

#ifdef H323_H224
    PBoolean H224ReceivedSettings(const PString & id, BYTE cpid, const BYTE * data, int & length);

    void SetH224Active(const PString & id);
    H224_Handler * GetH224Handler(const PString & id);
#endif

    void FECC(const PString & key, const PString & direction, const PString & start);
#ifdef H224_H281
    void SendH281Start(H281_Frame::PanDirection panDirection,
    H281_Frame::TiltDirection tiltDirection,
    H281_Frame::ZoomDirection zoomDirection,
    H281_Frame::FocusDirection focusDirection);
    void SendH281Stop();
#endif

#if defined(H224_T140) || defined(H323_T140) 
    void ReceivedT140Text(const PString & text);
    void ReceivedT140BackSpace();

    void SendT140BackSpace();
    void SendT140Character(const PString & ch);
#endif

    void SendH284Instruction(int id, int action, int value);
#ifdef H224_H284
    void ReceiveH284Instruction(H224_H284Handler::ControlPointID id, H284_ControlPoint::Action action, unsigned value) const;
    PBoolean OnCreateH284ControlID(H224_H284Handler::ControlPointID id, H284_ControlPoint & cp);
#endif

    PStringList GetConfigurationSections() const;
    PConfig *   GetConfigurationRules(const PString & section);

private:
    PlusProcess & m_process;                    // Process for callbacks

    PString     m_currentCallToken;             // Current call token Not empty when on a call
    bool        m_activeCall;                   // Whether there is an active call going on

    PString     m_configFile;                   // Configuration File
    PStringList m_configList;                   // Configuration List


    int         m_stunType;                     // STUN type for H.460.18/.19
    PString     m_h46017GW;                     // H.460.17 Tunneling gateway

#ifdef H224_H281
    PMutex      m_feccMutex;                    // FECC MUTEX
#endif

#ifdef H323_H235_AES256
    PThread *       m_generateThread;
    PDECLARE_NOTIFIER(PThread, PlusEndPoint, ThreadGenerate);
#endif

    PBoolean    m_endpointIsSetup;
    PDirectory  m_libPath;

    PlusMediaManager  m_mediaManager;

#ifdef H323_DATASTORE
    H323DataStore & m_dataStore;                // Settings for the endpoint
#endif
};


/////////////////////////////////////////////////////////////////////////////////////////////

class PlusConnection : public H323Connection
{
    PCLASSINFO(PlusConnection, H323Connection);

public:
    PlusConnection(PlusEndPoint &, unsigned);

    PBoolean OnReceivedSignalSetup(const H323SignalPDU & pdu);
    PBoolean OnReceivedSignalConnect(const H323SignalPDU & pdu);

#ifdef H323_H224
    H224_Handler * CreateH224Handler(H323Channel::Directions dir, OpalH224Handler & h224Handler, const PString & id);
    PBoolean OnCreateH224Handler(H323Channel::Directions dir, const PString & id, H224_Handler * m_handler) const;

    PBoolean SetH224Handler(const PString & id, H224_Handler * handler, H323Channel::Directions dir);
    H224_Handler * GetH224Handler(const PString & id);
#endif

    // Internals
    void HandleExceptions();

protected:

#ifdef H323_H224
    map<PString, H224_Handler*> m_h224handlerMap;
#endif
    PlusEndPoint & myEndpoint;

};

