//------------------------------------------------------------------------
// Copyright(c) 2021-2022 Olli Vanhoja.
//------------------------------------------------------------------------

#include "processor.h"
#include "process.h"
#include "cids.h"
#include "paramids.h"

#include "base/source/fstreamer.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

namespace MyVst {
using namespace Steinberg;
using namespace Steinberg::Vst;

DemonProProcessor::DemonProProcessor ()
{
    //--- set the wanted controller for our processor
    setControllerClass (kDemonProControllerUID);
}

DemonProProcessor::~DemonProProcessor ()
{}

tresult PLUGIN_API DemonProProcessor::initialize (FUnknown* context)
{
    //---always initialize the parent-------
    tresult result = AudioEffect::initialize (context);
    if (result != kResultOk) {
        return result;
    }

    //--- create Audio IO ------
    addAudioInput(STR16("Mono In"), Steinberg::Vst::SpeakerArr::kMono);
    addAudioOutput(STR16("Mono Out"), Steinberg::Vst::SpeakerArr::kMono);

    /*
     * We want to grab MIDI CC events from the channel 2 so we need to add
     * two channels and the channel 2 will be in index 2.
     * TODO Any needed?
     */
    addEventInput(STR16("Event In"), 2);

    return kResultOk;
}

tresult PLUGIN_API DemonProProcessor::terminate ()
{
    //---do not forget to call parent ------
    return AudioEffect::terminate();
}

tresult PLUGIN_API DemonProProcessor::setActive (TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
    if (state) {
        float sampleRate = (float)this->processSetup.sampleRate;

        dees32.updateParams(sampleRate);
        dees32.reset();
        comp32.updateParams(sampleRate);
        comp32.reset();

        dees64.updateParams(sampleRate);
        dees64.reset();
        comp64.updateParams(sampleRate);
        comp64.reset();

        /*
         * Note that the initialization order is different from the other
         * effect processors.
         */
        exct32.reset(sampleRate);
        exct32.updateParams();

        exct64.reset(sampleRate);
        exct64.updateParams();
    }

    /*
     * Reset VU
     */
    memset(fVuPPMInOld, 0, sizeof(fVuPPMInOld));
    memset(fVuPPMOutOld, 0, sizeof(fVuPPMOutOld));

	return AudioEffect::setActive (state);
}

void DemonProProcessor::handleParamChanges(IParameterChanges* paramChanges)
{
    /*if (data.inputParameterChanges)
    {
        int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
        for (int32 index = 0; index < numParamsChanged; index++)
        {
            if (auto* paramQueue = data.inputParameterChanges->getParameterData (index))
            {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount ();
                switch (paramQueue->getParameterId ())
                {
				}
			}
		}
	}*/
	int32 numParamsChanged = paramChanges->getParameterCount ();
    bool compChanged = false;
    bool deesChanged = false;
    bool exctChanged = false;

    // for each parameter which are some changes in this audio block:
    for (int32 i = 0; i < numParamsChanged; i++) {
        IParamValueQueue* paramQueue = paramChanges->getParameterData(i);
		if (paramQueue) {
			ParamValue value;
            int32 sampleOffset;
            int32 numPoints = paramQueue->getPointCount();

            switch (paramQueue->getParameterId ()) {
                case kBypassId:
                    if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) == kResultTrue) {
                        bBypass = value > 0.5f;
                    }
                    break;

                case kGainId:
                    // we use in this example only the last point of the queue.
                    // in some wanted case for specific kind of parameter it makes sense to
                    // retrieve all points and process the whole audio block in small blocks.
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        fGain = value;
                    }
                    break;

                case kCompThreshId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        comp32.thresh = value;
                        comp64.thresh = value;
                        compChanged = true;
                    }
                    break;

                case kCompAttimeId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        comp32.attime = value;
                        comp64.attime = value;
                        compChanged = true;
                    }
                    break;

                case kCompReltimeId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        comp32.reltime = value;
                        comp64.reltime = value;
                        compChanged = true;
                    }
                    break;

                case kCompRatioId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        comp32.ratio = value;
                        comp64.ratio = value;
                        compChanged = true;
                    }
                    break;

                case kCompKneeId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        comp32.knee = value;
                        comp64.knee = value;
                        compChanged = true;
                    }
                    break;

                case kCompMakeupId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        comp32.makeup = value;
                        comp64.makeup = value;
                        compChanged = true;
                    }
                    break;

                case kCompMixId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        comp32.mix = value;
                        comp64.mix = value;
                        compChanged = true;
                    }
                    break;

                case kCompLookAheadId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        comp32.lookahead = value;
                        comp64.lookahead = value;
                        compChanged = true;
                    }
                    break;

                case kCompStereoLinkId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        comp32.stereo_link = value > 0.5f;
                        comp64.stereo_link = value > 0.5f;
                    }
                    break;

                case kCompEnabledId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        comp32.enabled = value > 0.5f;
                        comp64.enabled = value > 0.5f;
                    }
                    break;

                case kDeEsserThreshId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        dees32.thresh = value;
                        dees64.thresh = value;
                        deesChanged = true;
                    }
                    break;

                case kDeEsserFreqId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        dees32.freq = value;
                        dees64.freq = value;
                        deesChanged = true;
                    }
                    break;

                case kDeEsserDriveId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        dees32.drive = value;
                        dees64.drive = value;
                        deesChanged = true;
                    }
                    break;

                case kDeEsserEnabledId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        dees32.enabled = value > 0.5f;
                        dees64.enabled = value > 0.5f;
                    }
                    break;

                case kExciterDriveId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        exct32.drive = value;
                        exct64.drive = value;
                        exctChanged = true;
                    }
                    break;

                case kExciterFcId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        exct32.fc = value;
                        exct64.fc = value;
                        exctChanged = true;
                    }
                    break;

                case kExciterSatId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        exct32.sat = value;
                        exct64.sat = value;
                        exctChanged = true;
                    }
                    break;

                case kExciterBlendId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        exct32.blend = value;
                        exct64.blend = value;
                        exctChanged = true;
                    }
                    break;

                case kExciterEnabledId:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                        exct32.enabled = value > 0.5f;
                        exct64.enabled = value > 0.5f;
                    }
                    break;
            }
        }
    }
}

tresult PLUGIN_API DemonProProcessor::process (Vst::ProcessData& data)
{
	/*
     * Read inputs parameter changes.
     */
    IParameterChanges* paramChanges = data.inputParameterChanges;
    if (paramChanges) {
        handleParamChanges(paramChanges);
    }

    /*
     * Processing
     */

    if (data.numInputs != 1 || data.numOutputs != 1) {
        return kResultOk;
    }

    void* in = getChannelBuffersPointer(processSetup, data.inputs[0][0]);
    void* out = getChannelBuffersPointer(processSetup, data.outputs[0][0]);
    const int nrSamples = data.numSamples;
    const size_t sampleFramesSize = getSampleFramesSizeInBytes(processSetup, nrSamples);

    if (data.inputs[0].silenceFlags) {
        data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

        if (in != out) {
            memset(out, 0, sampleFramesSize);
        }
    } else {
        // Normally the output is not silenced
        data.outputs[0].silenceFlags = 0;

        if (bBypass) {
            memmove(out, in, sampleFramesSize);
        } else {
            void *buf;

            buf = malloc(sampleFramesSize);
            if (!buf) {
                return kResultFalse; // TODO Is this the right code?
            }

            for (int i = 0; i < 3) {
                memcpy(buf, in, sampleFramesSize);

                // TODO Add hardcoded HPF?
                if (data.symbolicSampleSize == kSample32) {
                    demon32.process((Sample32*)buf, nrSamples);
                } else {
                    demon64.process((Sample64*)buf, nrSamples);
                }
            }
        }
    }

	return kResultOk;
}

tresult PLUGIN_API DemonProProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	//--- called before any processing ----
	return AudioEffect::setupProcessing(newSetup);
}

tresult PLUGIN_API DemonProProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	if (symbolicSampleSize == Vst::kSample32 || symbolicSampleSize == Vst::kSample64)
		return kResultTrue;

	return kResultFalse;
}

tresult PLUGIN_API DemonProProcessor::setBusArrangements (Steinberg::Vst::SpeakerArrangement* inputs, int32 numIns,
                                                        Steinberg::Vst::SpeakerArrangement* outputs, int32 numOuts)
{
    if (numIns == 1 && numOuts == 1 &&
        Steinberg::Vst::SpeakerArr::getChannelCount(inputs[0]) == 1 &&
        Steinberg::Vst::SpeakerArr::getChannelCount(outputs[0]) == 1) {
        auto* bus = FCast<Steinberg::Vst::AudioBus>(audioInputs.at (0));
        if (bus) {
            // check if we are Mono => Mono, if not we need to recreate the busses
            if (bus->getArrangement() != inputs[0]) {
                getAudioInput(0)->setArrangement(inputs[0]);
                getAudioInput(0)->setName(STR16("Mono In"));
                getAudioOutput(0)->setArrangement(inputs[0]);
                getAudioOutput(0)->setName (STR16("Mono Out"));
            }
            return kResultOk;
        }
    }
    return kResultFalse;
}

tresult PLUGIN_API DemonProProcessor::setState (IBStream* state)
{
    // called when we load a preset or project, the model has to be reloaded
    if (!state) {
        kResultFalse;
    }

	IBStreamer streamer(state, kLittleEndian);

    int32 savedBypass = 0;

    if (!streamer.readInt32(savedBypass)) {
            return kResultFalse;
    }

    bBypass = savedBypass > 0;
    float sampleRate = (float)this->processSetup.sampleRate;

    exct32.drive = savedExciterDrive;
    exct32.fc = savedExciterFc;
    exct32.sat = savedExciterSat;
    exct32.blend = savedExciterBlend;
    exct32.enabled = savedExciterEnabled;
    exct32.updateParams();

    exct64.drive = savedExciterDrive;
    exct64.fc = savedExciterFc;
    exct64.sat = savedExciterSat;
    exct64.blend = savedExciterBlend;
    exct64.enabled = savedExciterEnabled;
    exct64.updateParams();

	return kResultOk;
}

tresult PLUGIN_API DemonProProcessor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

    /*
     * YOLO, we just write the state in the right order here
     * and hope it goes well.
     */
    streamer.writeInt32(bBypass ? 1 : 0);

	return kResultOk;
}

}
