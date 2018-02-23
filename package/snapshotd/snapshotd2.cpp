/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2016, Live Networks, Inc.  All rights reserved
// A demo application, showing how to create and run a RTSP client (that can potentially receive multiple streams concurrently).
//
// NOTE: This code - although it builds a running application - is intended only to illustrate how to develop your own RTSP
// client application.  For a full-featured RTSP client application - with much more functionality, and many options - see
// "openRTSP": http://www.live555.com/openRTSP/

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <syslog.h>
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

UsageEnvironment* env = NULL;
RTSPClient *rtspClient = NULL;
static bool io_to_syslog = false;
static int debug_level = 0;
EventTriggerId snapshotTriggerId = 0;
const char *snapshotPrefix = "";
const char *snapshotFormat = "%Y%m%d-%H%M%S.jpg";

static int indicatorLedGPIO = -1;
TaskToken indicatorLedTask = NULL;

char eventLoopWatchVariable = 0;

// Forward function definitions:

// Indicator LED
void initializeGPIO();
void turnOnLedIndicator();
void turnOffLedIndicator();

// RTSP 'response handlers':
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);

// Other event handler functions:
void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
void subsessionByeHandler(void* clientData); // called when a RTCP "BYE" is received for a subsession
void streamTimerHandler(void* clientData);
  // called at the end of a stream's expected duration (if the stream has not already signaled its end using a RTCP "BYE")

// The main streaming routine (for each "rtsp://" URL):
void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL);

// Used to iterate through each stream's 'subsessions', setting up each one:
void setupNextSubsession(RTSPClient* rtspClient);

// Used to shut down and close a stream (including its "RTSPClient" object):
void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);

// A function that outputs a string that identifies each stream (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const RTSPClient& rtspClient) {
  return env << "[" << rtspClient.url() << "]: ";
}

// A function that outputs a string that identifies each subsession (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const MediaSubsession& subsession) {
  return env << subsession.mediumName() << "/" << subsession.codecName();
}

// Define a class to hold per-stream state that we maintain throughout each stream's lifetime:

class StreamClientState {
public:
  StreamClientState();
  virtual ~StreamClientState();

public:
  MediaSubsessionIterator* iter;
  MediaSession* session;
  MediaSubsession* subsession;
  TaskToken streamTimerTask;
  double duration;
};

// If you're streaming just a single stream (i.e., just from a single URL, once), then you can define and use just a single
// "StreamClientState" structure, as a global variable in your application.  However, because - in this demo application - we're
// showing how to play multiple streams, concurrently, we can't do that.  Instead, we have to have a separate "StreamClientState"
// structure for each "RTSPClient".  To do this, we subclass "RTSPClient", and add a "StreamClientState" field to the subclass:

class ourRTSPClient: public RTSPClient {
public:
  static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
				  int verbosityLevel = 0,
				  char const* applicationName = NULL,
				  portNumBits tunnelOverHTTPPortNum = 0);

  void resetStreamState();

protected:
  ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
    // called only by createNew();
  virtual ~ourRTSPClient();

public:
  StreamClientState scs;

private:
  char *fOriginalURL;
};

// Define a data sink (a subclass of "MediaSink") to receive the data for each subsession (i.e., each audio or video 'substream').
// In practice, this might be a class (or a chain of classes) that decodes and then renders the incoming audio or video.
// Or it might be a "FileSink", for outputting the received data into a file (as is done by the "openRTSP" application).
// In this example code, however, we define a simple 'dummy' sink that receives incoming data, but does nothing with it.

class DummySink: public MediaSink {
public:
  static DummySink* createNew(UsageEnvironment& env,
			      MediaSubsession& subsession, // identifies the kind of data that's being received
			      char const* streamId = NULL); // identifies the stream itself (optional)

private:
  DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId);
    // called only by "createNew()"
  virtual ~DummySink();

  static void afterGettingFrame(void* clientData, unsigned frameSize,
                                unsigned numTruncatedBytes,
				struct timeval presentationTime,
                                unsigned durationInMicroseconds);
  void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
			 struct timeval presentationTime, unsigned durationInMicroseconds);

private:
  // redefined virtual functions:
  virtual Boolean continuePlaying();

private:
  u_int8_t* fReceiveBuffer;
  MediaSubsession& fSubsession;
  char* fStreamId;
};

void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL) {
  // Begin by creating a "RTSPClient" object.  Note that there is a separate "RTSPClient" object for each stream that we wish
  // to receive (even if more than stream uses the same "rtsp://" URL).
  rtspClient = ourRTSPClient::createNew(env, rtspURL, debug_level, progName);
  if (rtspClient == NULL) {
    env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
    return;
  }

  // Next, send a RTSP "DESCRIBE" command, to get a SDP description for the stream.
  // Note that this command - like all RTSP commands - is sent asynchronously; we do not block, waiting for a response.
  // Instead, the following function call returns immediately, and we handle the RTSP response later, from within the event loop:
  //rtspClient->sendDescribeCommand(continueAfterDESCRIBE);
}

static void reconnectTimerHandler(void *clientData) {
  RTSPClient* rtspClient = (RTSPClient*)clientData;
  rtspClient->sendDescribeCommand(continueAfterDESCRIBE);
}

// Implementation of the RTSP 'response handlers':

void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) {
  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
      delete[] resultString;
      break;
    }

    char* const sdpDescription = resultString;
    if (debug_level) {
      env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";
    }

    // Create a media session object from this SDP description:
    scs.session = MediaSession::createNew(env, sdpDescription);
    delete[] sdpDescription; // because we don't need it anymore
    if (scs.session == NULL) {
      env << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
      break;
    } else if (!scs.session->hasSubsessions()) {
      env << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
      break;
    }

    // Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
    // calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
    // (Each 'subsession' will have its own data source.)
    scs.iter = new MediaSubsessionIterator(*scs.session);
    setupNextSubsession(rtspClient);
    return;
  } while (0);

  // An unrecoverable error occurred with this stream.
  UsageEnvironment& env = rtspClient->envir();
  int64_t uSecsToDelay = 2000000; // delay 2 seconds to reconnect
  env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)reconnectTimerHandler, rtspClient);
}

// By default, we request that the server stream its data using RTP/UDP.
// If, instead, you want to request that the server stream via RTP-over-TCP, change the following to True:
#define REQUEST_STREAMING_OVER_TCP False

void setupNextSubsession(RTSPClient* rtspClient) {
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

  scs.subsession = scs.iter->next();
  if (scs.subsession != NULL) {
    if (!scs.subsession->initiate()) {
      env << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
      setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
    } else {
      if (debug_level) {
        env << *rtspClient << "Initiated the \"" << *scs.subsession << "\" subsession (";
        if (scs.subsession->rtcpIsMuxed()) {
          env << "client port " << scs.subsession->clientPortNum();
        } else {
          env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1;
        }
        env << ")\n";
      }

      // Continue setting up this subsession, by sending a RTSP "SETUP" command:
      rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
    }
    return;
  }

  // We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
  if (scs.session->absStartTime() != NULL) {
    // Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());
  } else {
    scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
  }
}

void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) {
  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      env << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\n";
      break;
    }

    if (debug_level) {
      env << *rtspClient << "Set up the \"" << *scs.subsession << "\" subsession (";
      if (scs.subsession->rtcpIsMuxed()) {
        env << "client port " << scs.subsession->clientPortNum();
      } else {
        env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1;
      }
      env << ")\n";
    }

    // Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
    // (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
    // after we've sent a RTSP "PLAY" command.)

    scs.subsession->sink = DummySink::createNew(env, *scs.subsession, rtspClient->url());
      // perhaps use your own custom "MediaSink" subclass instead
    if (scs.subsession->sink == NULL) {
      env << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession
	  << "\" subsession: " << env.getResultMsg() << "\n";
      break;
    }

    if (debug_level) {
      env << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";
    }

    scs.subsession->miscPtr = rtspClient; // a hack to let subsession handler functions get the "RTSPClient" from the subsession
    scs.subsession->sink->startPlaying(*(scs.subsession->readSource()),
				       subsessionAfterPlaying, scs.subsession);
    // Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
    if (scs.subsession->rtcpInstance() != NULL) {
      scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.subsession);
    }
  } while (0);
  delete[] resultString;

  // Set up the next subsession, if any:
  setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) {
  Boolean success = False;

  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

    if (resultCode != 0) {
      env << *rtspClient << "Failed to start playing session: " << resultString << "\n";
      break;
    }

    // Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
    // using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
    // 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
    // (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
    if (scs.duration > 0) {
      unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
      scs.duration += delaySlop;
      unsigned uSecsToDelay = (unsigned)(scs.duration*1000000);
      scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
    }

    if (debug_level) {
      env << *rtspClient << "Started playing session";
      if (scs.duration > 0) {
        env << " (for up to " << scs.duration << " seconds)";
      }
      env << "...\n";
    }

    success = True;
  } while (0);
  delete[] resultString;

  if (!success) {
    // An unrecoverable error occurred with this stream.
    ((ourRTSPClient*)rtspClient)->resetStreamState();
  }
}


// Implementation of the other event handlers:

void subsessionAfterPlaying(void* clientData) {
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

  // Begin by closing this subsession's stream:
  Medium::close(subsession->sink);
  subsession->sink = NULL;

  // Next, check whether *all* subsessions' streams have now been closed:
  MediaSession& session = subsession->parentSession();
  MediaSubsessionIterator iter(session);
  while ((subsession = iter.next()) != NULL) {
    if (subsession->sink != NULL) return; // this subsession is still active
  }

  // All subsessions' streams have now been closed, so shutdown the client:
  ((ourRTSPClient*)rtspClient)->resetStreamState();
}

void subsessionByeHandler(void* clientData) {
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
  UsageEnvironment& env = rtspClient->envir(); // alias

  if (debug_level) {
    env << *rtspClient << "Received RTCP \"BYE\" on \"" << *subsession << "\" subsession\n";
  }

  // Now act as if the subsession had closed:
  subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void* clientData) {
  ourRTSPClient* rtspClient = (ourRTSPClient*)clientData;
  StreamClientState& scs = rtspClient->scs; // alias

  scs.streamTimerTask = NULL;

  // Shut down the stream:
  ((ourRTSPClient*)rtspClient)->resetStreamState();
}

void shutdownStream(RTSPClient* rtspClient, int exitCode) {
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

  // First, check whether any subsessions have still to be closed:
  if (scs.session != NULL) {
    Boolean someSubsessionsWereActive = False;
    MediaSubsessionIterator iter(*scs.session);
    MediaSubsession* subsession;

    while ((subsession = iter.next()) != NULL) {
      if (subsession->sink != NULL) {
	Medium::close(subsession->sink);
	subsession->sink = NULL;

	if (subsession->rtcpInstance() != NULL) {
	  subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
	}

	someSubsessionsWereActive = True;
      }
    }

    if (someSubsessionsWereActive) {
      // Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
      // Don't bother handling the response to the "TEARDOWN".
      rtspClient->sendTeardownCommand(*scs.session, NULL);
    }
  }

  env << *rtspClient << "Closing the stream.\n";
  Medium::close(rtspClient);
    // Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.

  eventLoopWatchVariable = 1;
}


// Implementation of "ourRTSPClient":

ourRTSPClient* ourRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
					int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) {
  return new ourRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

ourRTSPClient::ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
			     int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
  : RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1),
    fOriginalURL(strDup(rtspURL))
{
}

ourRTSPClient::~ourRTSPClient() {
  delete[] fOriginalURL;
}

void ourRTSPClient::resetStreamState()
{
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

  delete scs.iter; scs.iter = NULL; scs.subsession = NULL;

  // Check whether any subsessions have still to be closed:
  if (scs.session != NULL) {
    Boolean someSubsessionsWereActive = False;
    MediaSubsessionIterator iter(*scs.session);
    MediaSubsession* subsession;

    while ((subsession = iter.next()) != NULL) {
      if (subsession->sink != NULL) {
        Medium::close(subsession->sink);
        subsession->sink = NULL;
      }
    }

    env.taskScheduler().unscheduleDelayedTask(scs.streamTimerTask);
    Medium::close(scs.session); scs.session = NULL;
  }

  reset(); setBaseURL(fOriginalURL);

  int64_t uSecsToDelay = 100000; // delay 100ms to reconnect
  env.taskScheduler().scheduleDelayedTask(uSecsToDelay,
      (TaskFunc*)reconnectTimerHandler, rtspClient);
}


// Implementation of "StreamClientState":

StreamClientState::StreamClientState()
  : iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0) {
}

StreamClientState::~StreamClientState() {
  delete iter;
  if (session != NULL) {
    // We also need to delete "session", and unschedule "streamTimerTask" (if set)
    UsageEnvironment& env = session->envir(); // alias

    env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
    Medium::close(session);
  }
}


// Implementation of "DummySink":

// Even though we're not going to be doing anything with the incoming data, we still need to receive it.
// Define the size of the buffer that we'll use:
#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 200000

DummySink* DummySink::createNew(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId) {
  return new DummySink(env, subsession, streamId);
}

DummySink::DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId)
  : MediaSink(env),
    fSubsession(subsession) {
  fStreamId = strDup(streamId);
  fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
}

DummySink::~DummySink() {
  delete[] fReceiveBuffer;
  delete[] fStreamId;
}

void DummySink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
				  struct timeval presentationTime, unsigned durationInMicroseconds) {
  DummySink* sink = (DummySink*)clientData;
  sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

void DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
				  struct timeval presentationTime, unsigned /*durationInMicroseconds*/) {
  // We've just received a frame of data.  (Optionally) print out information about it:
  if (debug_level) {
    if (fStreamId != NULL) envir() << "[" << fStreamId << "] ";
    envir() << fSubsession.mediumName() << "/" << fSubsession.codecName()
            << ": Received " << frameSize << " bytes";
    if (numTruncatedBytes > 0)
      envir() << " (with " << numTruncatedBytes << " bytes truncated)";
    char uSecsStr[6+1]; // used to output the 'microseconds' part of the presentation time
    sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
    envir() << ".\tPresentation time: " << (int)presentationTime.tv_sec << "." << uSecsStr;
    envir() << "\n";
  }

  // PAUSE stream
  RTSPClient* rtspClient = (RTSPClient*)(fSubsession.miscPtr);
  rtspClient->sendPauseCommand(fSubsession, NULL);

  char filename[64] = { 0 };
  time_t t;
  struct tm *tmp;
  t = time(NULL);
  tmp = localtime(&t);
  if (tmp) {
    strftime(filename, sizeof(filename), snapshotFormat, tmp);
  }

  if (strlen(filename) > 0) {
    char path[256] = { 0 };
    snprintf(path, sizeof(path), "%s%s", snapshotPrefix, filename);

    // mkdir if directory not exists
    char *dpath = strdup(path);
    char *dname = dirname(dpath);
    if (access(dname, F_OK) != 0) {
      char cmd[256];
      snprintf(cmd, sizeof(cmd), "mkdir -p \"%s\"", dname);
      system(cmd);
    }
    free(dpath);

    envir() << "Saving " << frameSize << " bytes of snapshot to " << path << "\n";

    FILE *fp = fopen(path, "w+");
    if (fp) {
      fwrite(fReceiveBuffer, frameSize, 1, fp);
      fclose(fp);
    }
  }

  // Then continue, to request the next frame of data:
  continuePlaying();
}

Boolean DummySink::continuePlaying() {
  if (fSource == NULL) return False; // sanity check (should not happen)

  // Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
  fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
                        afterGettingFrame, this,
                        onSourceClosure, this);
  return True;
}


void usage(UsageEnvironment& env, char const* progName) {
  env << "Usage: " << progName << " [OPTIONS] <rtsp-url-1> ... <rtsp-url-N>\n";
  env << "  (where each <rtsp-url-i> is a \"rtsp://\" URL)\n"
         "OPTIONS:\n"
         "  -h = show this help text\n"
         "  -s = log output to syslog instead of stdout\n"
         "  -d = increase debugging verbosity\n"
         "  -l = specify indicator led gpio number\n"
         "  -p = path prefix of the filename\n"
         "  -f = format of filename\n";
}

void signalHandlerShutdown(int /*sig*/) {
  shutdownStream(rtspClient, 0);
}

void signalHandlerSnapshot(int sig) {
  rtspClient->envir().taskScheduler().triggerEvent(snapshotTriggerId, rtspClient);
}

static void indicatorLedTaskProc(void *clientData) {
  turnOffLedIndicator();
}

void takeSnapshot(void* clientData) {
  // Turn on indicator LED
  TaskScheduler &scheduler = rtspClient->envir().taskScheduler();
  scheduler.unscheduleDelayedTask(indicatorLedTask);
  turnOnLedIndicator();
  indicatorLedTask = scheduler.scheduleDelayedTask(200000, indicatorLedTaskProc, NULL);
  // Do taking picture
  *env << "Taking snapshot\n";
  StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs;

  if (scs.session)
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
  else
    rtspClient->sendDescribeCommand(continueAfterDESCRIBE);
}

static void redirect_io_to_syslog(FILE **pfp)
{
  cookie_io_functions_t io_funcs = {
    .read = [](void*, char*, size_t) -> ssize_t { return 0; },
    .write = [](void* cookie, const char* buf, size_t size) -> ssize_t {
       syslog(LOG_INFO, "%.*s", (int)size, buf);
       return size;
    },
    .seek = [](void*, off64_t*, int) -> int { return 0; },
    .close = [](void*) -> int { return 0; }
  };
  setvbuf(*pfp = fopencookie(NULL, "w", io_funcs), NULL, _IOLBF, 1024);
}

int main(int argc, char** argv) {
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  int c;
  while ((c = getopt(argc, argv, ":hsdl:f:p:")) != -1) {
    switch (c) {
    case 'h':
      usage(*env, argv[0]);
      return 0;
    case 's':
      io_to_syslog = true;
      break;
    case 'd':
      debug_level = 1;
      break;
    case 'l':
      indicatorLedGPIO = strtoul(optarg, NULL, 0);
      break;
    case 'p':
      snapshotPrefix = optarg;
      break;
    case 'f':
      snapshotFormat = optarg;
      break;
    case ':':
      *env << "Option -" << (char)c << "requires an operand\n";
      break;
    case '?':
      *env << "Unrecognized option: '-" << (char)c << "'\n";
      return -1;
    }
  }
  // We need at least one "rtsp://" URL argument:
  if (optind >= argc) {
    usage(*env, argv[optind]);
    return 1;
  }

  signal(SIGUSR1, signalHandlerSnapshot);
  signal(SIGUSR2, signalHandlerSnapshot);
  signal(SIGINT, signalHandlerShutdown);
  signal(SIGTERM, signalHandlerShutdown);
  signal(SIGQUIT, signalHandlerShutdown);

  if (io_to_syslog) {
    openlog("snapshotd", LOG_CONS, LOG_USER);
    redirect_io_to_syslog(&stdout);
    redirect_io_to_syslog(&stderr);
  }
  
  if (indicatorLedGPIO >= 0) {
  	initializeGPIO();
  	atexit(turnOffLedIndicator);
  }

  // Open and start streaming:
  openURL(*env, argv[0], argv[optind]);

  snapshotTriggerId = scheduler->createEventTrigger(takeSnapshot);

  // All subsequent activity takes place within the event loop:
  env->taskScheduler().doEventLoop(&eventLoopWatchVariable);

  scheduler->deleteEventTrigger(snapshotTriggerId);

  env->reclaim(); env = NULL;
  delete scheduler; scheduler = NULL;

  if (io_to_syslog) {
    closelog();
  }

  return 0;
}

static int write_file(const char *fname, const char *str)
{
  int fd, bytes;

  if ((fd = open(fname, O_WRONLY)) < 0)
    return fd;

  bytes = write(fd, str, strlen(str));

  close(fd);

  return bytes;
}

void initializeGPIO() {
  char buf[32];
  char fname[128];

  sprintf(fname, "/sys/class/gpio/gpio%d/direction", indicatorLedGPIO);
  if (access(fname, R_OK) < 0) {
    sprintf(buf, "%d", indicatorLedGPIO);
    if (write_file("/sys/class/gpio/export", buf) < 0) {
      perror("Failed to export GPIO for record indicator");
      return;
    }
  }

  if (write_file(fname, "out") < 0) {
    perror("Failed to set GPIO to output");
  }

  turnOffLedIndicator();
}

void turnOnLedIndicator() {
  char fname[128];
  sprintf(fname, "/sys/class/gpio/gpio%d/value", indicatorLedGPIO);
  write_file(fname, "0");
}

void turnOffLedIndicator() {
  char fname[128];
  sprintf(fname, "/sys/class/gpio/gpio%d/value", indicatorLedGPIO);
  write_file(fname, "1");
}

